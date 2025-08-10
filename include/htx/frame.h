#ifndef HTX_FRAME_H
#define HTX_FRAME_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "aead.h"

#define HTX_MAX_FRAME 262144u

typedef enum {
  HTX_T_STREAM    = 0x00,
  HTX_T_ACK       = 0x01,
  HTX_T_FLOW      = 0x02,
  HTX_T_KEYUPDATE = 0x03,
  HTX_T_CLOSE     = 0x05,
  HTX_T_PADDING   = 0xFE
} htx_type_t;

typedef struct {
  uint8_t  type;
  uint64_t pkt_num;
  bool     has_stream;
  uint64_t stream_id;
  uint32_t pt_len;
} htx_hdr_info;

// Pack generic frame (STREAM/ etc.)
size_t htx_pack_frame(uint8_t *out, size_t out_cap,
                      uint8_t type,
                      uint64_t pkt_num,
                      bool has_stream, uint64_t stream_id,
                      const uint8_t *pt, uint32_t pt_len,
                      const uint8_t key[HTX_KEY_LEN],
                      const uint8_t salt[HTX_NONCE_LEN]);

// Unpack generic frame and decrypt into pt_out
int htx_unpack_frame(const uint8_t *buf, size_t len,
                     const uint8_t key[HTX_KEY_LEN],
                     const uint8_t salt[HTX_NONCE_LEN],
                     htx_hdr_info *info_out,
                     uint8_t *pt_out, size_t pt_cap);

#endif
