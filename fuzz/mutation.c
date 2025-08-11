// fuzz/fuzz_mutation.c
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "htx/htx.h"
#include "htx/errors.h"

static void kdf32_12(const uint8_t* in, size_t n, uint8_t out32[32], uint8_t out12[12]) {
  uint8_t acc[44] = {0};
  for (size_t i = 0; i < n; i++) acc[i % 44] ^= (uint8_t)(in[i] + i * 31);
  memcpy(out32, acc, 32);
  memcpy(out12, acc + 32, 12);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 8) return 0;

    size_t half = size / 2;
    uint8_t key[32], salt[12];
    kdf32_12(data, half, key, salt);

    htx_ctx_t ctx; htx_init(&ctx, key, salt, key, salt);

    uint64_t stream_id = (uint64_t)(data[0] & 0x3F);
    const uint8_t* pt = data + 1;
    size_t pt_len = (half > 1) ? (half - 1) : 0;
    if (pt_len > HTX_MAX_FRAME) pt_len = HTX_MAX_FRAME;

    uint8_t frame[HTX_MAX_FRAME + 64];
    size_t n = htx_pack_stream(&ctx, HTX_DIR_C2S, stream_id, pt, (uint32_t)pt_len, frame, sizeof(frame));
    if (n == 0) return 0;

    // Flip one byte selected from second half of input data
    size_t pos = (size - half > 0) ? ((size_t)data[half] % n) : 0;
    frame[pos] ^= (uint8_t)(data[half] | 1);

    // Unpack must not crash. It will usually return HTX_ERR_DECRYPT.
    htx_hdr_info info;
    uint8_t out[HTX_MAX_FRAME + 1];
    htx_unpack_any(&ctx, HTX_DIR_C2S, frame, n, &info, out, sizeof(out));

    return 0;
}
