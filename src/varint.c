#include "htx/varint.h"

size_t htx_qvar_write(uint8_t *out, uint64_t v) {
  if (v <= 0x3F) { out[0] = (uint8_t)(v & 0x3F); return 1; }
  else if (v <= 0x3FFF) {
    uint16_t w = (uint16_t)v;
    out[0] = 0x40 | (uint8_t)((w >> 8) & 0x3F);
    out[1] = (uint8_t)(w & 0xFF);
    return 2;
  } else if (v <= 0x3FFFFFFF) {
    uint32_t w = (uint32_t)v;
    out[0] = 0x80 | (uint8_t)((w >> 24) & 0x3F);
    out[1] = (uint8_t)((w >> 16) & 0xFF);
    out[2] = (uint8_t)((w >>  8) & 0xFF);
    out[3] = (uint8_t)( w        & 0xFF);
    return 4;
  } else {
    out[0] = 0xC0 | (uint8_t)((v >> 56) & 0x3F);
    out[1] = (uint8_t)((v >> 48) & 0xFF);
    out[2] = (uint8_t)((v >> 40) & 0xFF);
    out[3] = (uint8_t)((v >> 32) & 0xFF);
    out[4] = (uint8_t)((v >> 24) & 0xFF);
    out[5] = (uint8_t)((v >> 16) & 0xFF);
    out[6] = (uint8_t)((v >>  8) & 0xFF);
    out[7] = (uint8_t)( v        & 0xFF);
    return 8;
  }
}

size_t htx_qvar_read(const uint8_t *in, size_t inlen, uint64_t *out) {
  if (inlen == 0) return 0;
  uint8_t tag = in[0] >> 6;
  switch (tag) {
    case 0:
      *out = in[0] & 0x3F; return 1;
    case 1:
      if (inlen < 2) return 0;
      *out = ((uint64_t)(in[0] & 0x3F) << 8) | in[1];
      if (*out <= 0x3F) return 0;
      return 2;
    case 2:
      if (inlen < 4) return 0;
      *out = ((uint64_t)(in[0] & 0x3F) << 24) |
             ((uint64_t)in[1] << 16) | ((uint64_t)in[2] << 8) | in[3];
      if (*out <= 0x3FFF) return 0;
      return 4;
    case 3:
      if (inlen < 8) return 0;
      *out = ((uint64_t)(in[0] & 0x3F) << 56) |
             ((uint64_t)in[1] << 48) | ((uint64_t)in[2] << 40) |
             ((uint64_t)in[3] << 32) | ((uint64_t)in[4] << 24) |
             ((uint64_t)in[5] << 16) | ((uint64_t)in[6] << 8)  | in[7];
      if (*out <= 0x3FFFFFFF) return 0;
      return 8;
  }
  return 0;
}
