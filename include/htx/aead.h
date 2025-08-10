#ifndef HTX_AEAD_H
#define HTX_AEAD_H
#include <stddef.h>
#include <stdint.h>

#define HTX_KEY_LEN   32
#define HTX_NONCE_LEN 12
#define HTX_TAG_LEN   16

int htx_aead_seal(const uint8_t key[HTX_KEY_LEN], const uint8_t nonce[HTX_NONCE_LEN],
                  const uint8_t *aad, size_t aad_len,
                  const uint8_t *pt, size_t pt_len,
                  uint8_t *ct, uint8_t tag[HTX_TAG_LEN]);

int htx_aead_open(const uint8_t key[HTX_KEY_LEN], const uint8_t nonce[HTX_NONCE_LEN],
                  const uint8_t *aad, size_t aad_len,
                  const uint8_t *ct, size_t ct_len,
                  const uint8_t tag[HTX_TAG_LEN],
                  uint8_t *pt_out);

#endif
