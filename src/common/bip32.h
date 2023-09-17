#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * 
 * Maximum length of BIP32 path allowed.
 * 
 */
#define MAX_BIP32_PATH 10

/**
 * Read BIP32 path from byte buffer.
 *
 * @param[in]  in
 *   Pointer to input byte buffer.
 * @param[in]  in_len
 *   Length of input byte buffer.
 * @param[out] out
 *   Pointer to output 32-bit integer buffer.
 * @param[in]  out_len
 *   Number of BIP32 paths read in the output buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool bip32_path_read(const uint8_t *in, size_t in_len, uint32_t *out, size_t out_len);
