#ifndef X25519_UTILS_H_
#define X25519_UTILS_H_

#include <openssl/evp.h>

EVP_PKEY* generate_keypair();

unsigned char* get_public_key(EVP_PKEY* pkey);
unsigned char* get_private_key(EVP_PKEY* pkey);

#endif // !X25519_UTILS_H_