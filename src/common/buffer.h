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
 * Buffer can read bytes or not.
 *
 * @param[in] buffer
 *   Pointer to input buffer struct.
 * @param[in] n
 *   Number of bytes to read in buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool buffer_can_read(const buffer_t *buffer, size_t n);

/**
 * Seek buffer relatively to current offset.
 *
 * @param[in,out] buffer
 *   Pointer to input buffer struct.
 * @param[in]     offset
 *   Offset to seek relatively to `buffer->offset`.
 *
 * @return true if success, false otherwise.
 *
 */
bool buffer_seek_cur(buffer_t *buffer, size_t offset);

/**
 * Read 1 byte from buffer into uint8_t.
 *
 * @param[in,out]  buffer
 *   Pointer to input buffer struct.
 * @param[out]     value
 *   Pointer to 8-bit unsigned integer read from buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool buffer_read_u8(buffer_t *buffer, uint8_t *value);

/**
 * Read BIP32 path from buffer.
 *
 * @param[in,out]  buffer
 *   Pointer to input buffer struct.
 * @param[out]     out
 *   Pointer to output 32-bit integer buffer.
 * @param[in]      out_len
 *   Number of BIP32 paths read in the output buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool buffer_read_bip32_path(buffer_t *buffer, uint32_t *out, size_t out_len);

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