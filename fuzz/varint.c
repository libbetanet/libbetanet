#include <stdint.h>
#include <stddef.h>

#include "htx/varint.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size == 0) {
        return 0;
    }

        uint64_t dummy = 0;
        htx_qvar_read(data, i, &dummy);
    }

    uint64_t v = 0;
    size_t n = htx_qvar_read(data, size, &v);
    
    if (n > 0) {
        uint8_t enc_buf[8];
        size_t m = htx_qvar_write(enc_buf, v);

        if (n != m) {
            __builtin_trap();
        }
    }

    return 0; // Success
}
