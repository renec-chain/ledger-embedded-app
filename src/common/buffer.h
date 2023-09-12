#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Struct for buffer with size and offset.
 */
typedef struct {
    const uint8_t *ptr;  // Pointer to byte buffer
    size_t size;         // Size of byte buffer
    size_t offset;       // Offset in byte buffer
} buffer_t;

/**
 * Copy bytes from buffer without moving offset.
 *
 * @param[in]  buffer
 *   Pointer to input buffer struct.
 * @param[out] out
 *   Pointer to output byte buffer.
 * @param[in]  out_len
 *   Length of output byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool buffer_copy(const buffer_t *buffer, uint8_t *out, size_t out_len);