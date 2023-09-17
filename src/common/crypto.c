#include "exceptions.h"
#include "os_seed.h"

#include "crypto.h"

int crypto_derive_private_key(cx_ecfp_private_key_t *private_key,
                              const uint32_t *bip32_path,
                              uint8_t bip32_path_len) {
    uint8_t raw_private_key[RAW_ED25519_PRIVATE_KEY_SIZE] = {0};
    int error = 0;

    BEGIN_TRY {
        TRY {
            // TODO Implement seed phrase derivation from BIP32 path
            // derive seed with bip32_path
            // os_perso_derive_node_with_seed_key();
            // new private_key from raw
            // cx_ecfp_init_private_key();
        }
        CATCH_OTHER(e) {
            error = e;
        }
        FINALLY {
            explicit_bzero(&raw_private_key, sizeof(raw_private_key));
        }
    }
    END_TRY;

    return error;
}

// converts little endian 32 byte public key to big endian 32 byte public key
void raw_public_key_le_to_be(cx_ecfp_public_key_t *public_key,
                             uint8_t raw_public_key[static RAW_ED25519_PUBLIC_KEY_SIZE]) {
    // copy public key little endian to big endian
    for (uint8_t i = 0; i < RAW_ED25519_PUBLIC_KEY_SIZE; i++) {
        raw_public_key[i] = public_key->W[64 - i];
    }

    // set sign bit
    if ((public_key->W[32] & 1) != 0) {
        raw_public_key[31] |= 0x80;
    }
}

void crypto_init_public_key(cx_ecfp_private_key_t *private_key,
                            cx_ecfp_public_key_t *public_key,
                            uint8_t raw_public_key[static RAW_ED25519_PUBLIC_KEY_SIZE]) {
    // generate corresponding public key
    cx_ecfp_generate_pair(CX_CURVE_Ed25519, public_key, private_key, 1);
    raw_public_key_le_to_be(public_key, raw_public_key);
}
