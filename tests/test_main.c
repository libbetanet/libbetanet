#include "htx/errors.h"
#include "htx/htx.h"
#include <inttypes.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  const uint8_t key[32] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                           0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                           0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                           0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f};
  const uint8_t salt[12] = {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
                            0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab};

  htx_ctx_t ctx;
  if (htx_init(&ctx, key, salt, key, salt) != 0) {
    fprintf(stderr, "init fail\n");
    return 1;
  }

  const uint8_t msg[] = "hello";
  uint8_t frame[1024];
  size_t fsz = htx_pack_stream(&ctx, HTX_DIR_C2S, /*stream_id=*/1, msg,
                               (uint32_t)sizeof(msg) - 1, frame, sizeof(frame));
  if (fsz == 0) {
    fprintf(stderr, "pack fail\n");
    return 2;
  }

  uint8_t out[1024];
  htx_hdr_info info;
  int r =
      htx_unpack_any(&ctx, HTX_DIR_C2S, frame, fsz, &info, out, sizeof(out));
  if (r < 0) {
    fprintf(stderr, "unpack fail: %d\n", r);
    return 3;
  }
  out[r] = 0;

  printf("Decoded type=0x%02x pkt=%" PRIu64 " stream=%" PRIu64
         " len=%u msg='%s'\n",
         info.type, info.pkt_num, info.stream_id, info.pt_len, out);

  // Tamper header: flip type -> should fail
  frame[3] ^= 0x01;
  r = htx_unpack_any(&ctx, HTX_DIR_C2S, frame, fsz, &info, out, sizeof(out));
  if (r != HTX_ERR_DECRYPT) {
    fprintf(stderr, "tamper expected decrypt error, got %d\n", r);
    return 4;
  }
  printf("Tamper test OK\n");

  const uint8_t reason[] = "test reason";
  uint8_t frame_close[1024];
  size_t fsz_close =
      htx_pack_close(&ctx, HTX_DIR_C2S, 0xdeadbeef, sizeof(reason) - 1, reason,
                     frame_close, sizeof(frame_close));
  if (fsz_close == 0) {
    fprintf(stderr, "pack fail\n");
    return 2;
  }

  uint8_t out_close[1024];
  htx_hdr_info info_close;
  int r_close = htx_unpack_any(&ctx, HTX_DIR_C2S, frame_close, fsz_close,
                               &info_close, out_close, sizeof(out_close));
  if (r_close < 0) {
    fprintf(stderr, "unpack fail: %d\n", r);
    return 3;
  }

  htx_print_hdr_info(&info_close, out_close, r_close);

  return 0;
}
