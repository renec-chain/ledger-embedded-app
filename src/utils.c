#include "os.h"
#include "cx.h"
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"

void get_public_key(uint8_t *publicKeyArray, const uint32_t *derivationPath, size_t pathLength) {
    cx_ecfp_private_key_t privateKey;
    cx_ecfp_public_key_t publicKey;

    get_private_key(&privateKey, derivationPath, pathLength);
    BEGIN_TRY {
        TRY { cx_ecfp_generate_pair_no_throw(CX_CURVE_Ed25519, &publicKey, &privateKey, 1); }
        CATCH_OTHER(e) {
            MEMCLEAR(privateKey);
            THROW(e);
        }
        FINALLY { MEMCLEAR(privateKey); }
    }
    END_TRY;

    for (int i = 0; i < PUBKEY_LENGTH; i++) {
        publicKeyArray[i] = publicKey.W[PUBKEY_LENGTH + PRIVATEKEY_LENGTH - i];
    }
    if ((publicKey.W[PUBKEY_LENGTH] & 1) != 0) {
        publicKeyArray[PUBKEY_LENGTH - 1] |= 0x80;
    }
}

uint32_t readUint32BE(uint8_t *buffer) {
    return ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]));
}

void get_private_key(cx_ecfp_private_key_t *privateKey,
                     const uint32_t *derivationPath,
                     size_t pathLength) {
    cx_err_t error = CX_OK;
    uint8_t privateKeyData[PRIVATEKEY_LENGTH];
    BEGIN_TRY {
        TRY {
            // Derive private key according to BIP32 path
            CX_CHECK(os_derive_bip32_with_seed_no_throw(HDW_ED25519_SLIP10,
                                                        CX_CURVE_Ed25519,
                                                        derivationPath,
                                                        pathLength,
                                                        privateKeyData,
                                                        NULL,
                                                        NULL,
                                                        0));

            // Init privkey from raw
            cx_ecfp_init_private_key_no_throw(CX_CURVE_Ed25519,
                                              privateKeyData,
                                              PRIVATEKEY_LENGTH,
                                              privateKey);
        end:
            explicit_bzero(privateKeyData, sizeof(privateKeyData));

            if (error != CX_OK) {
                // Make sure the caller doesn't use uninitialized data in case
                // the return code is not checked.
                explicit_bzero(privateKeyData, sizeof(cx_ecfp_256_private_key_t));
            }
        }
        CATCH_OTHER(e) {
            MEMCLEAR(privateKeyData);
            THROW(e);
        }
        FINALLY { MEMCLEAR(privateKeyData); }
    }
    END_TRY;
}

void get_private_key_with_seed(cx_ecfp_private_key_t *privateKey,
                               const uint32_t *derivationPath,
                               uint8_t pathLength) {
    cx_err_t error = CX_OK;
    uint8_t privateKeyData[PRIVATEKEY_LENGTH];
    BEGIN_TRY {
        TRY {
            CX_CHECK(os_derive_bip32_with_seed_no_throw(HDW_ED25519_SLIP10,
                                                        CX_CURVE_Ed25519,
                                                        derivationPath,
                                                        pathLength,
                                                        privateKeyData,
                                                        NULL,
                                                        (unsigned char *) "ed25519 seed",
                                                        12));

            cx_ecfp_init_private_key_no_throw(CX_CURVE_Ed25519,
                                              privateKeyData,
                                              PRIVATEKEY_LENGTH,
                                              privateKey);
        end:
            explicit_bzero(privateKeyData, sizeof(privateKeyData));

            if (error != CX_OK) {
                // Make sure the caller doesn't use uninitialized data in case
                // the return code is not checked.
                explicit_bzero(privateKeyData, sizeof(cx_ecfp_256_private_key_t));
            }
        }
        CATCH_OTHER(e) {
            MEMCLEAR(privateKeyData);
            THROW(e);
        }
        FINALLY { MEMCLEAR(privateKeyData); }
    }
    END_TRY;
}

int read_derivation_path(const uint8_t *data_buffer,
                         size_t data_size,
                         uint32_t *derivation_path,
                         uint32_t *derivation_path_length) {
    if (!data_buffer || !derivation_path || !derivation_path_length) {
        return ApduReplySdkInvalidParameter;
    }
    if (!data_size) {
        return ApduReplySolanaInvalidMessageSize;
    }
    const size_t len = data_buffer[0];
    data_buffer += 1;
    if (len < 1 || len > MAX_BIP32_PATH_LENGTH) {
        return ApduReplySolanaInvalidMessage;
    }
    if (1 + 4 * len > data_size) {
        return ApduReplySolanaInvalidMessageSize;
    }

    for (size_t i = 0; i < len; i++) {
        derivation_path[i] = ((data_buffer[0] << 24u) | (data_buffer[1] << 16u) |
                              (data_buffer[2] << 8u) | (data_buffer[3]));
        data_buffer += 4;
    }

    *derivation_path_length = len;

    return 0;
}

uint8_t set_result_sign_message(void) {
    uint8_t signature[SIGNATURE_LENGTH];
    cx_ecfp_private_key_t privateKey;
    BEGIN_TRY {
        TRY {
            get_private_key_with_seed(&privateKey,
                                      G_command.derivation_path,
                                      G_command.derivation_path_length);
            cx_eddsa_sign_no_throw(&privateKey,
                                   CX_SHA512,
                                   G_command.message,
                                   G_command.message_length,
                                   signature,
                                   SIGNATURE_LENGTH);
            memcpy(G_io_apdu_buffer, signature, SIGNATURE_LENGTH);
        }
        CATCH_OTHER(e) { THROW(e); }
        FINALLY { MEMCLEAR(privateKey); }
    }
    END_TRY;
    return SIGNATURE_LENGTH;
}
