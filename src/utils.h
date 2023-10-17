#ifndef _UTILS_H_
#define _UTILS_H_

#include "os.h"
#include "cx.h"
#include "globals.h"
#include <string.h>
#include "apdu.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

typedef enum rlpTxType {
    TX_LENGTH = 0,
    TX_TYPE,
    TX_SENDER,
    TX_RECIPIENT,
    TX_AMOUNT,
    TX_FEE
} rlpTxType;

void get_public_key(uint8_t *publicKeyArray, const uint32_t *derivationPath, size_t pathLength);

uint32_t readUint32BE(uint8_t *buffer);

void get_private_key(cx_ecfp_private_key_t *privateKey,
                     const uint32_t *derivationPath,
                     size_t pathLength);

void get_private_key_with_seed(cx_ecfp_private_key_t *privateKey,
                               const uint32_t *derivationPath,
                               uint8_t pathLength);

/**
 * Deserialize derivation path from raw bytes.
 *
 * @param[in] data_buffer
 *   Pointer to serialized bytes.
 * @param[in] data_size
 *   Size of the data_buffer.
 * @param[out] derivation_path
 *   Pointer to the target array to store deserialized data into.
 * @param[out] derivation_path_length
 *   Pointer to the variable that will hold derivation path length.
 *
 * @return zero on success, ApduReply error code otherwise.
 *
 */
int read_derivation_path(const uint8_t *data_buffer,
                         size_t data_size,
                         uint32_t *derivation_path,
                         uint32_t *derivation_path_length);

uint8_t set_result_sign_message(void);

#endif  //_UTILS_H_

// Outdated ?
#ifdef TEST
#include <stdio.h>
#define THROW(code)                \
    do {                           \
        printf("error: %d", code); \
    } while (0)
#define PRINTF(msg, arg) printf(msg, arg)
#define PIC(code)        code
#define TARGET_BLUE      1
#define MEMCLEAR(dest)   explicit_bzero(&dest, sizeof(dest));
#else
#define MEMCLEAR(dest)                       \
    do {                                     \
        explicit_bzero(&dest, sizeof(dest)); \
    } while (0)
#include "bolos_target.h"
#endif  // UTILS_H
