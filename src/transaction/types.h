#pragma once

#define RAW_ED25519_PUBLIC_KEY_SIZE  32
#define RAW_ED25519_PRIVATE_KEY_SIZE 32

typedef enum {
    ENVELOPE_TYPE_TX = 2,
    ENVELOPE_TYPE_TX_FEE_BUMP = 5,
} envelope_type_e;

// typedef struct {
//     muxed_account_t fee_source;
//     int64_t fee;
// } fee_bump_transaction_details_t;