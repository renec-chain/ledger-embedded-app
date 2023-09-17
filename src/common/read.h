#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * Read 4 bytes as Big Endian from byte buffer.
 *
 * @param[in] ptr
 *   Pointer to byte buffer.
 * @param[in] offset
 *   Offset in the byte buffer.
 *
 * @return 4 bytes value read from buffer.
 *
 */
uint32_t read_u32_be(const uint8_t *ptr, size_t offset);
