// fuzz/fuzz_pack_roundtrip.c
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "htx/htx.h"
#include "htx/errors.h"
#include "htx/varint.h"

// Deterministic key/salt derivation from input (not crypto).
static void kdf32_12(const uint8_t* in, size_t n, uint8_t out32[32], uint8_t out12[12]) {
  uint8_t acc[44] = {0};
  for (size_t i = 0; i < n; i++) acc[i % 44] ^= (uint8_t)(in[i] + i * 131);
  memcpy(out32, acc, 32);
  memcpy(out12, acc + 32, 12);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 4) return 0;

    uint8_t key[32], salt[12];
    kdf32_12(data, size, key, salt);

    const uint8_t types[] = { HTX_T_STREAM, HTX_T_FLOW, HTX_T_CLOSE, HTX_T_PADDING };
    uint8_t ftype = types[data[0] % (sizeof(types) / sizeof(types[0]))];

    htx_ctx_t ctx;
    htx_init(&ctx, key, salt, key, salt);

    uint64_t pkt = (uint64_t)data[1];
    uint64_t stream_id = (uint64_t)(data[2] & 0x3F);
    const uint8_t* p = data + 3;
    size_t plen = size - 3;
    if (plen > HTX_MAX_FRAME) plen = HTX_MAX_FRAME;

    uint8_t body_buf[1024];
    const uint8_t *body = body_buf;
    uint32_t blen = 0;
    bool has_stream = false;

    switch (ftype) {
        case HTX_T_STREAM:
        case HTX_T_PADDING:
            has_stream = (ftype == HTX_T_STREAM);
            body = p; blen = (uint32_t)plen;
            break;
        case HTX_T_FLOW:
            has_stream = true;
            blen = htx_qvar_write(body_buf, (uint64_t)plen);
            break;
        case HTX_T_CLOSE: {
            uint64_t code = (p[0] % 16);
            blen = htx_qvar_write(body_buf, code);
            blen += htx_qvar_write(body_buf + blen, (uint64_t)0); // empty reason
            break;
        }
        default: return 0;
    }

    uint8_t frame[HTX_MAX_FRAME + 64];
    size_t n = htx_pack_frame(frame, sizeof(frame), ftype, pkt, has_stream, stream_id, body, blen, key, salt);
    if (n == 0) return 0;

    htx_hdr_info info;
    uint8_t out[HTX_MAX_FRAME + 1];
    int r = htx_unpack_frame(frame, n, key, salt, &info, out, sizeof(out));
    if (r < 0) __builtin_trap(); // Unpacking a self-packed frame should never fail.

    if (info.type != ftype) __builtin_trap();
    if (info.pkt_num != pkt) __builtin_trap();
    if (info.has_stream != has_stream) __builtin_trap();
    if (has_stream && info.stream_id != stream_id) __builtin_trap();
    if ((size_t)r != blen) __builtin_trap();
    if (memcmp(out, body, blen) != 0) __builtin_trap();
    
    return 0;
}
