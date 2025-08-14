#include "htx/frame.h"
#include "htx/u24.h"
#include "htx/varint.h"
#include "htx/nonce.h"
#include "htx/errors.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

size_t htx_pack_frame(uint8_t *out, size_t out_cap,
                      uint8_t type, uint64_t pkt_num,
                      bool has_stream, uint64_t stream_id,
                      const uint8_t *pt, uint32_t pt_len,
                      const uint8_t key[HTX_KEY_LEN],
                      const uint8_t salt[HTX_NONCE_LEN]) {
  if (pt_len > HTX_MAX_FRAME) return 0;
  uint8_t hdr[3 + 1 + 8 + 8]; size_t hdr_len = 0;
  htx_u24be_write(hdr, pt_len); hdr_len += 3;
  hdr[hdr_len++] = type;
  hdr_len += htx_qvar_write(hdr + hdr_len, pkt_num);
  if (has_stream) hdr_len += htx_qvar_write(hdr + hdr_len, stream_id);
  size_t need = hdr_len + pt_len + HTX_TAG_LEN;
  if (need > out_cap) return 0;
  memcpy(out, hdr, hdr_len);
  uint8_t nonce[HTX_NONCE_LEN]; htx_make_nonce(nonce, salt, pkt_num);
  uint8_t *ct = out + hdr_len; uint8_t *tag = ct + pt_len;
  if (!htx_aead_seal(key, nonce, out, hdr_len, pt, pt_len, ct, tag)) return 0;
  return need;
}

int htx_unpack_frame(const uint8_t *buf, size_t len,
                     const uint8_t key[HTX_KEY_LEN],
                     const uint8_t salt[HTX_NONCE_LEN],
                     htx_hdr_info *info_out,
                     uint8_t *pt_out, size_t pt_cap) {
  if (len < 4) return HTX_ERR_SHORT;
  uint32_t ct_len=0; if (!htx_u24be_read(buf, &ct_len)) return HTX_ERR_LENCAP;
  if (ct_len > HTX_MAX_FRAME) return HTX_ERR_LENCAP;
  uint8_t type = buf[3];
  size_t off = 4;
  uint64_t pkt_num=0; size_t c = htx_qvar_read(buf+off, len-off, &pkt_num);
  if (c == 0) {
    return HTX_ERR_VARINT;
  }
  off += c;
  bool has_stream = (type == HTX_T_STREAM || type == HTX_T_FLOW);
  uint64_t stream_id = 0;
  if (has_stream) {
    c = htx_qvar_read(buf+off, len-off, &stream_id);
    if (c == 0) {
        return HTX_ERR_STREAMID;
    }
    off += c;
  }
  size_t needed = off + ct_len + HTX_TAG_LEN;
  if (len < needed) return HTX_ERR_INCOMPLETE;
  if (pt_cap < ct_len) return HTX_ERR_DEST_SMALL;
  const uint8_t *aad = buf; size_t aad_len = off;
  const uint8_t *ct = buf + off; const uint8_t *tag = ct + ct_len;
  uint8_t nonce[HTX_NONCE_LEN]; htx_make_nonce(nonce, salt, pkt_num);
  if (!htx_aead_open(key, nonce, aad, aad_len, ct, ct_len, tag, pt_out)) return HTX_ERR_DECRYPT;

  if (info_out) {
    info_out->type = type; info_out->pkt_num = pkt_num;
    info_out->has_stream = has_stream; info_out->stream_id = stream_id;
    info_out->pt_len = ct_len;
  }
  return (int)ct_len;
}

void htx_print_hdr_info(const htx_hdr_info *info, uint8_t *out,
                        uint32_t out_len) {
  printf("PACKET\ttype:\t");

  switch (info->type) {
  case HTX_T_STREAM:
    printf("STREAM");
    break;
  case HTX_T_CLOSE:
    printf("CLOSE");
    break;
  case HTX_T_FLOW:
    printf("FLOW");
    break;
  case HTX_T_ACK:
    printf("ACK");
    break;
  case HTX_T_KEYUPDATE:
    printf("KEYUPDATE");
    break;
  case HTX_T_PADDING:
    printf("PADDING");
    break;
  default:
    printf("unknown");
    break;
  }

  printf("\n\tpacket number: %llu", info->pkt_num);

  if (info->has_stream) {
    printf("\n\tstream id: %llu\t", info->stream_id);
  } else {
    printf("\n\tstream id: none\t");
  }

  printf("\n\tmessage len: %u", out_len);
  printf("\n\tmessage (txt): ");
  for (uint32_t i = 0; i < out_len; i++) {
    if(isprint(out[i])){
      putchar(out[i]);
    }else {
      printf("\\%03u ", out[i]);
    }
  }
  printf("\n\tmessage (hex): \n\t\t");
  for (uint32_t i = 0; i < out_len; i++) {
    printf("0x%02x ", out[i]);
    if ((i + 1) % 8 == 0) {
      printf("\n\t\t");
    }
  }
  puts("\n");
}

