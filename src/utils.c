#include "cx.h"
#include "exceptions.h"
#include "os_seed.h"

#include "utils.h"

#include "apdu/types.h"

#include "common/utils.h"

void get_public_key(uint8_t *publicKeyArray, const uint32_t *derivationPath, size_t pathLength) {
    cx_ecfp_private_key_t privateKey;
    cx_ecfp_public_key_t publicKey;

    get_private_key(&privateKey, derivationPath, pathLength);
    BEGIN_TRY {
        TRY {
            cx_ecfp_generate_pair(CX_CURVE_Ed25519, &publicKey, &privateKey, 1);
        }
        CATCH_OTHER(e) {
            MEMCLEAR(privateKey);
            THROW(e);
        }
        FINALLY {
            MEMCLEAR(privateKey);
        }
    }
    END_TRY;

    for (int i = 0; i < PUBKEY_LENGTH; i++) {
        publicKeyArray[i] = publicKey.W[PUBKEY_LENGTH + PRIVATEKEY_LENGTH - i];
    }
    if ((publicKey.W[PUBKEY_LENGTH] & 1) != 0) {
        publicKeyArray[PUBKEY_LENGTH - 1] != 0x80;
    }   
}

void get_private_key(cx_ecfp_private_key_t *privateKey,
                     const uint32_t *derivationPath,
                     size_t pathLength) {
    uint8_t privateKeyData[PRIVATEKEY_LENGTH];
    BEGIN_TRY {
        TRY {
            os_perso_derive_node_bip32_seed_key(HDW_ED25519_SLIP10,
                                                CX_CURVE_Ed25519,
                                                derivationPath,
                                                pathLength,
                                                privateKeyData,
                                                NULL,
                                                NULL,
                                                0);
            cx_ecfp_init_private_key(CX_CURVE_Ed25519,
                                     privateKeyData,
                                     PRIVATEKEY_LENGTH,
                                     privateKey);
        }
        CATCH_OTHER(e) {
            MEMCLEAR(privateKeyData);
            THROW(e);
        }
        FINALLY {
            MEMCLEAR(privateKeyData);
        }
    }
    END_TRY;
}

int read_derivation_path(const uint8_t *data_buffer,
                         size_t data_size,
                         uint32_t *derivation_path,
                         uint32_t *derivation_path_length) {
    if (!data_buffer || !derivation_path || !derivation_path_length) {
        return SDK_INVALID_PARAMETER;
    }
    if (!data_size) {
        return RENEC_INVALID_MESSAGE_SIZE;
    }
    const size_t len = data_buffer[0];
    data_buffer += 1;
    if (len < 1 || len > MAX_BIP32_PATH_LENGTH) {
        return RENEC_INVALID_MESSAGE;
    }
    if (1 + 4 * len > data_size) {
        return RENEC_INVALID_MESSAGE_SIZE;
    }

    for (size_t i = 0; i < len; i++) {
        derivation_path[i] = ((data_buffer[0] << 24u) | (data_buffer[1] << 16u) | (data_buffer[2] << 8u) | (data_buffer[3]));
        data_buffer += 4;
    }

    *derivation_path_length = len;
    
    return 0;
}

static const char BASE58_ALPHABET[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
                                       'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
                                       'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c',
                                       'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
                                       'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

int encode_base58(const void *in, size_t length, char *out, size_t maxoutlen) {
    uint8_t tmp[64];
    uint8_t buffer[64];
    uint8_t j;
    size_t start_at;
    size_t zero_count = 0;
    if (length > sizeof(tmp)) {
        return INVALID_PARAMETER;
    }
    memmove(tmp, in, length);
    while ((zero_count < length) && (tmp[zero_count] == 0)) {
        ++zero_count;
    }
    j = 2 * length;
    start_at = zero_count;
    while (start_at < length) {
        uint16_t remainder = 0;
        size_t div_loop;
        for (div_loop = start_at; div_loop < length; div_loop++) {
            uint16_t digit256 = (uint16_t) (tmp[div_loop] & 0xff);
            uint16_t tmp_div = remainder * 256 + digit256;
            tmp[div_loop] = (uint8_t) (tmp_div / 58);
            remainder = (tmp_div % 58);
        }
        if (tmp[start_at] == 0) {
            ++start_at;
        }
        buffer[--j] = (uint8_t) BASE58_ALPHABET[remainder];
    }
    while ((j < (2 * length)) && (buffer[j] == BASE58_ALPHABET[0])) {
        ++j;
    }
    while (zero_count-- > 0) {
        buffer[--j] = BASE58_ALPHABET[0];
    }
    length = 2 * length - j;
    if (maxoutlen < length + 1) {
        return EXCEPTION_OVERFLOW;
    }
    memmove(out, (buffer + j), length);
    out[length] = '\0';
    return 0;
}