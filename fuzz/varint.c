// fuzz/fuzz_varint.c

#include <stdint.h>
#include <stddef.h>

// These are C headers, no 'extern "C"' needed.
#include "htx/varint.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size == 0) {
        return 0;
    }

    // Test for clean failures on every possible truncated length
    for (size_t i = 1; i < size; i++) {
        uint64_t dummy = 0;
        // This must not crash, just return 0.
        htx_qvar_read(data, i, &dummy);
    }

    // Now test the full input
    uint64_t v = 0;
    size_t n = htx_qvar_read(data, size, &v);
    
    // If the read was successful, perform the canonicality check.
    if (n > 0) {
        uint8_t enc_buf[8];
        size_t m = htx_qvar_write(enc_buf, v);

        // This is the critical check. If the decoder accepted an inefficient
        // encoding, the number of bytes read (n) will not equal the number
        // of bytes required by the canonical encoder (m). This is a bug.
        if (n != m) {
            // In C, __builtin_trap() is the standard way to crash for a fuzzer.
            __builtin_trap();
        }
    }

    return 0; // Success
}
