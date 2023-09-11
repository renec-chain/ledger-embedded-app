#pragma once

#include <stdint.h>

#define HASH_SIZE 32

typedef struct {
    uint8_t data[HASH_SIZE];
} Hash;