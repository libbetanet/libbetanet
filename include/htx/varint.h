#ifndef HTX_VARINT_H
#define HTX_VARINT_H
#include <stdint.h>
#include <stddef.h>

// QUIC varint (RFC 9000 §16) canonical
size_t htx_qvar_write(uint8_t *out, uint64_t v);
size_t htx_qvar_read(const uint8_t *in, size_t inlen, uint64_t *out);

#endif
