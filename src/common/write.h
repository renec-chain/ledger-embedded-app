#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * 
 * Write 16-bit unsigned integer value as Big Indian.
 * 
 * @param[out] ptr
 *   Pointer to output byte buffer.
 * @param[in] offset
 *   Offset in the output byte buffer.
 * @param[in] value
 *   16-bit unsigned integer to write in output byte buffer as Big Indian.
 * 
 */
void write_u16_be(uint8_t *ptr, size_t offset, uint16_t value);