#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "htx/htx.h"
#include "htx/errors.h"

// parse hex into bytes
static int parse_hex(const char *hex, uint8_t *out, size_t expect) {
  size_t n = strlen(hex);
  if (n != expect*2) return -1;
  for (size_t i=0;i<expect;i++) {
    unsigned x; if (sscanf(&hex[i*2], "%2x", &x)!=1) return -1;
    out[i] = (uint8_t)x;
  }
  return 0;
}

static void usage(const char *arg0) {
  fprintf(stderr, "Usage: %s --dir c2s|s2c --key <64hex> --salt <24hex> <frame.bin>\n", arg0);
}

int main(int argc, char **argv) {
  const char *dir_s = NULL, *key_hex = NULL, *salt_hex = NULL, *file = NULL;
  for (int i=1;i<argc;i++) {
    if (!strcmp(argv[i],"--dir") && i+1<argc) { dir_s = argv[++i]; }
    else if (!strcmp(argv[i],"--key") && i+1<argc) { key_hex = argv[++i]; }
    else if (!strcmp(argv[i],"--salt") && i+1<argc) { salt_hex = argv[++i]; }
    else if (argv[i][0] != '-') { file = argv[i]; }
    else { usage(argv[0]); return 1; }
  }
  if (!dir_s || !key_hex || !salt_hex || !file) { usage(argv[0]); return 1; }
  htx_dir_t dir = (!strcmp(dir_s,"c2s")) ? HTX_DIR_C2S : HTX_DIR_S2C;

  uint8_t key[32], salt[12];
  if (parse_hex(key_hex, key, 32) != 0 || parse_hex(salt_hex, salt, 12) != 0) {
    fprintf(stderr, "Bad key/salt hex\n"); return 1;
  }

  // read file
  FILE *fp = fopen(file, "rb"); if (!fp) { perror("fopen"); return 1; }
  fseek(fp, 0, SEEK_END); long sz = ftell(fp); fseek(fp, 0, SEEK_SET);
  if (sz <= 0 || sz > 1<<20) { fprintf(stderr, "Bad file size\n"); fclose(fp); return 1; }
  uint8_t *buf = malloc(sz);
  if (!buf) { fprintf(stderr, "out of memory\n"); return 1; }
  if (fread(buf, 1, sz, fp) != (size_t)sz) { fprintf(stderr, "short read\n"); return 1; }
  fclose(fp);


  htx_ctx_t ctx; htx_init(&ctx, key, salt, key, salt);
  uint8_t *pt = malloc(sz);
  htx_hdr_info info;
  int r = htx_unpack_any(&ctx, dir, buf, (size_t)sz, &info, pt, (size_t)sz);
  if (r < 0) { fprintf(stderr, "Decode error: %d\n", r); return 1; }

  // Print JSON-ish
  printf("{\"type\":%u,\"pkt_num\":%llu,\"has_stream\":%s,\"stream_id\":%llu,\"pt_len\":%u,\"pt_hex\":\"",
         info.type, (unsigned long long)info.pkt_num, info.has_stream?"true":"false",
         (unsigned long long)info.stream_id, info.pt_len);
  for (int i=0;i<r;i++) printf("%02x", pt[i]);
  printf("\"}\n");

  free(buf); free(pt);
  return 0;
}
