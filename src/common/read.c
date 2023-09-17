#include "read.h"

uint32_t read_u32_be(const uint8_t *ptr, size_t offset) {
    return (uint32_t) ptr[offset + 0] << 24 |
           (uint32_t) ptr[offset + 1] << 16 |
           (uint32_t) ptr[offset + 2] << 8 |
           (uint32_t) ptr[offset + 3] << 0;
}