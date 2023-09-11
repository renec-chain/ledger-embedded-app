#include "write.h"

void write_u16_be(uint8_t *ptr, size_t offset, uint16_t value) {
    ptr[offset + 0] = (uint8_t) (value >> 8);
    ptr[offset + 1] = (uint8_t) (value >> 0);
}