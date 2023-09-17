#pragma once

// typedef enum StatusWord {
//     // Sucessful response
//     SUCCESS = 0x9000,

//     /** 
//      * SDK* come from nanos-secure-sdk/include/os.h. Here we add the
//      * 0x68__ prefix that app_main() ORs into those values before sending then
//      * over the wire
//      */
//     SDK_INVALID_PARAMETER = 0x6801,
//     SDK_NOT_SUPPORTED = 0x6802,
//     SDK_EXCEPTION_IO_OVERFLOW = 0x6803,
    // SDK_EXCEPTION_IO_RESET = 0x6804,

//     NO_APDU_RECEIVED = 0x6901,
//     USER_REFUSAL = 0x6902,

//     /**
//      * We preserve 0xB0__ for RENEC errors
//     */
//     RENEC_INVALID_MESSAGE = 0xB001,
//     RENEC_INVALID_MESSAGE_HEADER = 0xB002,
//     RENEC_INVALID_MESSAGE_FORMAT = 0xB003,
//     RENEC_INVALID_MESSAGE_SIZE = 0xB004,
//     RENEC_SUMMARY_FINALIZED_FAILED = 0xB005,
//     RENEC_SUMMARY_UPDATE_FAILED = 0xB006,

//     UNIMPLEMENTED_INSTRUCTION = 0x6D00,
//     // INVALID_CLA = 0x6E00,

//     // // Start of error which trigger automatic response
//     // // Note that any such error will reset
//     // // multi-APDU exchange
//     // _ERR_AUTORESPOND_START = 0x6E00,

//     // // Bad request header
//     // ERR_MALFORMED_REQUEST_HEADER = 0x6E01,
//     // // Unknown CLA
//     // ERR_BAD_CLA = 0x6E02,
//     // // Unknown INS
//     // ERR_UNKNOWN_INS = 0x6E03,
//     // // Attempt to change INS while the current call was not finished
//     // ERR_STILL_IN_CALL = 0x6E04,
//     // // P1 or P2 is invalid
//     // ERR_INVALID_REQUEST_PARAMETERS = 0x6E05,
//     // // Request is not valid in the context of previous calls
//     // ERR_INVALID_STATE = 0x6E06,
//     // // Some part of request data is invalid (or unknown to this app)
//     // // (includes not enough data and too much data)
//     // ERR_INVALID_DATA = 0x6E07,
//     // // User rejected the action
//     // ERR_REJECTED_BY_USER = 0x6E09,
//     // // Ledger security policy rejected the action
//     // ERR_REJECTED_BY_POLICY = 0x6E10,

//     // // Pin screen
//     // ERR_DEVICE_LOCKED = 0x6E11,

//     // Status word for wrong response length (buffer too small or too big).
//     SW_WRONG_RESPONSE_LENGTH 0xB000,
//     // Status word for instruction class is different than CLA.
//     SW_CLA_NOT_SUPPORTED 0x6E00,
// } StatusWord;

/**
 * Status word for either wrong Lc or minimum APDU lenght is incorrect.
 */
#define SW_WRONG_DATA_LENGTH 0x6A87

/**
 * Status word for wrong transaction length.
 */
#define SW_WRONG_TX_LENGTH 0xB004

/**
 * 
 * Status word for wrong response length (buffer too small or too big).
 * 
 */
#define SW_WRONG_RESPONSE_LENGTH 0xB000

/**
 * Status word for fail of transaction hash.
 */
#define SW_TX_HASH_FAIL 0xB006

/**
 * Status word for bad state.
 */
#define SW_BAD_STATE 0xB007

/**
 * Status word for unknown command with this INS.
 */
#define SW_INS_NOT_SUPPORTED 0x6D00

/**
 * 
 * Status word for instruction class is different than CLA.
 * 
 */ 
#define SW_CLA_NOT_SUPPORTED 0x6E00

/**
 * 
 * Status word for incorrect P1 or P2
 * 
 */
#define SW_WRONG_P1P2 0x6B00

/**
 * Status word for success.
 */
#define SW_OK 0x9000