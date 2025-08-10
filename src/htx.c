#include "htx/htx.h"
#include "htx/varint.h"
#include <string.h>

int htx_init(htx_ctx_t *ctx,
    const uint8_t c2s_key[HTX_KEY_LEN], const uint8_t c2s_salt[HTX_NONCE_LEN],
    const uint8_t s2c_key[HTX_KEY_LEN], const uint8_t s2c_salt[HTX_NONCE_LEN]) {
    if (!ctx) return -1;

    memcpy(ctx->c2s.key, c2s_key, HTX_KEY_LEN);
    memcpy(ctx->c2s.salt, c2s_salt, HTX_NONCE_LEN);
    ctx->c2s.next_pkt_num = 0;
    ctx->c2s.epoch = 0;

    memcpy(ctx->s2c.key, s2c_key, HTX_KEY_LEN);
    memcpy(ctx->s2c.salt, s2c_salt, HTX_NONCE_LEN);
    ctx->s2c.next_pkt_num = 0;
    ctx->s2c.epoch = 0;

    ctx->max_frame = HTX_MAX_FRAME;
    return 0;
}

size_t htx_pack_stream(htx_ctx_t *ctx, htx_dir_t dir, uint64_t stream_id,
    const uint8_t *pt, uint32_t pt_len,
    uint8_t *out, size_t out_cap) {
    htx_dir_keys_t *dk = (dir == HTX_DIR_C2S) ? &ctx->c2s : &ctx->s2c;
    uint64_t pkt = dk->next_pkt_num++;
    return htx_pack_frame(out, out_cap, HTX_T_STREAM, pkt, true, stream_id,
        pt, pt_len, dk->key, dk->salt);
}

size_t htx_pack_close(htx_ctx_t *ctx, htx_dir_t dir, uint64_t close_code,
                      uint64_t reason_len, const uint8_t* reason, uint8_t *out,
                      size_t out_cap) {
  htx_dir_keys_t *dk = (dir == HTX_DIR_C2S) ? &ctx->c2s : &ctx->s2c;
  uint64_t pkt = dk->next_pkt_num++;

  uint8_t pt[8 + 8 + reason_len];
  uint32_t pt_len = htx_qvar_write(pt, close_code);
  pt_len += htx_qvar_write(&pt[pt_len], reason_len);
  memcpy(&pt[pt_len], reason, reason_len);

  return htx_pack_frame(out, out_cap, HTX_T_CLOSE, pkt, false, 0, pt, pt_len,
                        dk->key, dk->salt);
}

int htx_unpack_any(htx_ctx_t *ctx, htx_dir_t dir,
    const uint8_t *buf, size_t len,
    htx_hdr_info *info_out,
    uint8_t *pt_out, size_t pt_cap) {
    const htx_dir_keys_t *dk = (dir == HTX_DIR_C2S) ? &ctx->c2s : &ctx->s2c;
    return htx_unpack_frame(buf, len, dk->key, dk->salt, info_out, pt_out, pt_cap);
}

