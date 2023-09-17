#pragma once

#include <stddef.h>
#include <stdint.h>

#include "cx.h"
// #include "lcx_ecfp.h"

void get_public_key(uint8_t *publicKeyArray, const uint32_t *derivationPath, size_t pathLength);

void get_private_key(cx_ecfp_private_key_t *privateKey,
                     const uint32_t *derivationPath,
                     size_t pathLength);
/**
 * Deserialize derivation path from raw bytes.
 *
 * @param[in] data_buffer
 *   Pointer to serialized bytes.
 * @param[in] data_size
 *   Size of the data_buffer.
 * @param[out] derivation_path
 *   Pointer to the target array to store deserialized data into.
 * @param[out] derivation_path_length
 *   Pointer to the variable that will hold derivation path length.
 *
 * @return zero on success, ApduReply error code otherwise.
 *
 */
int read_derivation_path(const uint8_t *data_buffer,
                         size_t data_size,
                         uint32_t *derivation_path,
                         uint32_t *derivation_path_length);

int encode_base58(const void *in, size_t length, char *out, size_t maxoutlen);
