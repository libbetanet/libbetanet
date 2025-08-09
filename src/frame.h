#pragma once

#include "int.h"

#define CIPHERTEXT_SIZE 65535

typedef struct {
    uint24_t length; // should always be CIPHERTEXT_SIZE ?
    uint8_t type;
    uint64_t stream_id;
    uint8_t ciphertext[CIPHERTEXT_SIZE];
} BN_FRAME;
