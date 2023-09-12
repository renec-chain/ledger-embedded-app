#pragma once

#include <stdint.h>

#include <os_pic.h>

typedef uint8_t internal_storage_t;

/**
 * 
 * The settings, stored in NVRAM. Initializer is ignored by ledger.
 * 
 */
extern const internal_storage_t N_storage_real;

#define N_settings (*(volatile internal_storage_t *) PIC(&N_storage_real))

// check a setting item
#define HAS_SETTING(k) ((N_settings & (1 << (k))) >> (k))

#define S_HASH_SIGNING_ENABLED 0

#define S_INITIALIZED 7