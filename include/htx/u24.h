#ifndef HTX_U24_H
#define HTX_U24_H
#include <stdint.h>
#include <stdbool.h>

static inline void htx_u24be_write(uint8_t out[3], uint32_t v) {
  out[0] = (uint8_t)((v >> 16) & 0xFF);
  out[1] = (uint8_t)((v >>  8) & 0xFF);
  out[2] = (uint8_t)( v        & 0xFF);
}
static inline bool htx_u24be_read(const uint8_t in[3], uint32_t *v) {
  *v = ((uint32_t)in[0] << 16) | ((uint32_t)in[1] << 8) | (uint32_t)in[2];
  return true;
}
#endif
