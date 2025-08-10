#include "htx/X25519_utils.h"

#include <openssl/evp.h>

EVP_PKEY* generate_keypair()
{
  EVP_PKEY *pkey = NULL;
  EVP_PKEY_CTX *pkey_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL);

  EVP_PKEY_keygen_init(pkey_ctx);
  EVP_PKEY_keygen(pkey_ctx, &pkey);

  EVP_PKEY_CTX_free(pkey_ctx);

  return pkey;
}

unsigned char* get_public_key(EVP_PKEY* pkey)
{
  unsigned char* public_key;
  size_t public_key_len;
  EVP_PKEY_get_raw_public_key(pkey, NULL, &public_key_len);
  public_key = malloc(public_key_len);
  EVP_PKEY_get_raw_public_key(pkey, public_key, &public_key_len);

  return public_key;
}

unsigned char* get_private_key(EVP_PKEY* pkey)
{
  unsigned char* private_key;
  size_t private_key_len;
  EVP_PKEY_get_raw_private_key(pkey, NULL, &private_key_len);
  private_key = malloc(private_key_len);
  EVP_PKEY_get_raw_private_key(pkey, private_key, &private_key_len);

  return private_key;
}