#ifndef HTX_HTX_H
#define HTX_HTX_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "aead.h"
#include "frame.h"

typedef enum { HTX_DIR_C2S = 0, HTX_DIR_S2C = 1 } htx_dir_t;

typedef struct {
  uint8_t key[HTX_KEY_LEN];
  uint8_t salt[HTX_NONCE_LEN];
  uint64_t next_pkt_num;
  uint8_t epoch;
} htx_dir_keys_t;

typedef struct {
  htx_dir_keys_t c2s;
  htx_dir_keys_t s2c;
  uint32_t max_frame; // cap, default 262144
} htx_ctx_t;

int    htx_init(htx_ctx_t *ctx,
                const uint8_t c2s_key[HTX_KEY_LEN], const uint8_t c2s_salt[HTX_NONCE_LEN],
                const uint8_t s2c_key[HTX_KEY_LEN], const uint8_t s2c_salt[HTX_NONCE_LEN]);

size_t htx_pack_stream(htx_ctx_t *ctx, htx_dir_t dir, uint64_t stream_id,
                       const uint8_t *pt, uint32_t pt_len,
                       uint8_t *out, size_t out_cap);

int    htx_unpack_any(htx_ctx_t *ctx, htx_dir_t dir,
                      const uint8_t *buf, size_t len,
                      htx_hdr_info *info_out,
                      uint8_t *pt_out, size_t pt_cap);

#endif
