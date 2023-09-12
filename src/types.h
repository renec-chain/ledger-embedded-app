#pragma once

#include "common/bip32.h"
#include "transaction/types.h"

#include "renec.h"

/**
 * Max transaction size (bytes).
 */
#ifdef TARGET_NANOS
#define RAW_TX_MAX_SIZE 1120
#else
#define RAW_TX_MAX_SIZE 5120
#endif

// /**
//  * 
//  * Enumeration for the status of IO.
//  * 
//  */
// typedef enum {
//     READY,      // ready for new event
//     RECEIVED,   // data received
//     WAITING,    // waiting
// } io_state_e;

/**
 * 
 * Enumeration with user request type.
 * 
 */
typedef enum {
    CONFIRM_ADDRESS,            // confirm address derived from public key
    CONFIRM_TRANSACTION,        // confirm transaction information
    CONFIRM_TRANSACTION_HASH,   // confirm transaction hash information
} request_type_e;

/**
 * 
 * Enumeration with parsing state.
 * 
 */
typedef enum {
    STATE_NONE,     // No state
    STATE_PARSED,   // Transaction data parsed
    STATE_APPROVED, // Transaction data approved
} state_e;

/**
 * Enumeration for the status of IO.
 */
typedef enum {
    READY,     // ready for new event
    RECEIVED,  // data received
    WAITING    // waiting
} io_state_e;

/**
 * Structure for transaction context.
 */
typedef struct {
    uint8_t raw[RAW_TX_MAX_SIZE];
    uint32_t raw_size;
    uint16_t offset;
    uint8_t network;
    envelope_type_e envelope_type;
    // fee_bump_transaction_details_t fee_bump_tx_details;
} tx_ctx_t;

/**
 * 
 * Structure for global context.
 * 
 */
typedef struct {
    tx_ctx_t tx_info;                                       // tx
    uint8_t hash[HASH_SIZE];                                // tx hash
    uint32_t bip32_path[MAX_BIP32_PATH];                    // BIP32 path
    uint8_t raw_public_key[RAW_ED25519_PUBLIC_KEY_SIZE];    // BIP32 path public key
    uint8_t bip32_path_len;                                 // length of BIP32 path
    state_e state;                                          // state of the context
    request_type_e req_type;                                // user request
} global_ctx_t;