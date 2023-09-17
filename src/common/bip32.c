#include "bip32.h"
#include "read.h"

bool bip32_path_read(const uint8_t *in, size_t in_len, uint32_t *out, size_t out_len) {
    if (out_len == 0 || out_len > MAX_BIP32_PATH) {
        return false;
    }

    size_t offset = 0;
    
    for (size_t i = 0; i < out_len; i++) {
        if (offset > in_len) {
            return false;
        }
        out[i] = read_u32_be(in, offset);
        offset += 4;
    }
    
    return true;
}