#include <stdint.h>
#include <string.h>

#include "buffer.h"

bool buffer_can_read(const buffer_t *buffer, size_t n) {
    return buffer->size - buffer->offset >= n;
}

bool buffer_seek_cur(buffer_t *buffer, size_t offset) {
    if (buffer->offset + offset < buffer->offset ||
        buffer->offset + offset > buffer->size) {
        return false;
    }

    buffer->offset += offset;

    return true;
}

bool buffer_read_u8(buffer_t *buffer, uint8_t *value) {
    if (!buffer_can_read(buffer, 1)) {
        *value = 0;
        
        return false;
    }

    *value = buffer->ptr[buffer->offset];
    buffer_seek_cur(buffer, 1);

    return true;
}

bool buffer_read_bip32_path(buffer_t *buffer, uint32_t *out, size_t out_len) {
    if (!bip32_path_read(buffer->ptr + buffer->offset,
                         buffer->size - buffer->offset,
                         out,
                         out_len)) {
        return false;
    }

    buffer_seek_cur(buffer, sizeof(*out) * out_len);

    return true;
}

bool buffer_copy(const buffer_t *buffer, uint8_t *out, size_t out_len) {
    if (buffer->size - buffer->offset > out_len) {
        return false;
    }

    memmove(out, buffer->ptr + buffer->offset, buffer->size - buffer->offset);

    return true;
}