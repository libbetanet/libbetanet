#ifndef HTX_NONCE_H
#define HTX_NONCE_H
#include <stdint.h>
#include "aead.h"

// nonce = salt XOR (LE64(pkt_num) || LE32(0))
static inline void htx_make_nonce(uint8_t nonce[HTX_NONCE_LEN], const uint8_t salt[HTX_NONCE_LEN], uint64_t pkt_num) {
  uint8_t buf[HTX_NONCE_LEN] = {0};
  buf[0] = (uint8_t)( pkt_num        & 0xFF);
  buf[1] = (uint8_t)((pkt_num >> 8 ) & 0xFF);
  buf[2] = (uint8_t)((pkt_num >> 16) & 0xFF);
  buf[3] = (uint8_t)((pkt_num >> 24) & 0xFF);
  buf[4] = (uint8_t)((pkt_num >> 32) & 0xFF);
  buf[5] = (uint8_t)((pkt_num >> 40) & 0xFF);
  buf[6] = (uint8_t)((pkt_num >> 48) & 0xFF);
  buf[7] = (uint8_t)((pkt_num >> 56) & 0xFF);
  for (int i = 0; i < HTX_NONCE_LEN; i++) nonce[i] = buf[i] ^ salt[i];
}
#endif
