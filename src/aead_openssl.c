#include "htx/aead.h"
#include <openssl/evp.h>

int htx_aead_seal(const uint8_t key[HTX_KEY_LEN], const uint8_t nonce[HTX_NONCE_LEN],
                  const uint8_t *aad, size_t aad_len,
                  const uint8_t *pt, size_t pt_len,
                  uint8_t *ct, uint8_t tag[HTX_TAG_LEN]) {
  int ok = 0, len = 0, outlen = 0;
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return 0;
  if (EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, NULL, NULL) != 1) goto out;
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, HTX_NONCE_LEN, NULL) != 1) goto out;
  if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, nonce) != 1) goto out;
  if (aad && aad_len) {
    if (EVP_EncryptUpdate(ctx, NULL, &len, aad, (int)aad_len) != 1) goto out;
  }
  if (pt_len) {
    if (EVP_EncryptUpdate(ctx, ct, &len, pt, (int)pt_len) != 1) goto out;
    outlen = len;
  }
  if (EVP_EncryptFinal_ex(ctx, ct + outlen, &len) != 1) goto out;
  outlen += len;
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, HTX_TAG_LEN, tag) != 1) goto out;
  ok = 1;
out:
  EVP_CIPHER_CTX_free(ctx);
  return ok;
}

int htx_aead_open(const uint8_t key[HTX_KEY_LEN], const uint8_t nonce[HTX_NONCE_LEN],
                  const uint8_t *aad, size_t aad_len,
                  const uint8_t *ct, size_t ct_len,
                  const uint8_t tag[HTX_TAG_LEN],
                  uint8_t *pt_out) {
  int ok = 0, len = 0, outlen = 0;
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return 0;
  if (EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, NULL, NULL) != 1) goto out;
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, HTX_NONCE_LEN, NULL) != 1) goto out;
  if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, nonce) != 1) goto out;
  if (aad && aad_len) {
    if (EVP_DecryptUpdate(ctx, NULL, &len, aad, (int)aad_len) != 1) goto out;
  }
  if (ct_len) {
    if (EVP_DecryptUpdate(ctx, pt_out, &len, ct, (int)ct_len) != 1) goto out;
    outlen = len;
  }
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, HTX_TAG_LEN, (void*)tag) != 1) goto out;
  if (EVP_DecryptFinal_ex(ctx, pt_out + outlen, &len) != 1) goto out;
  ok = 1;
out:
  EVP_CIPHER_CTX_free(ctx);
  return ok;
}
