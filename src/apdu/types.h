#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../constants.h"
#include "constants.h"

#include "../renec.h"

typedef enum {
    GET_VERSION = 0x03, /// version of the application
    GET_APP_NAME = 0x04, /// name of the application
    GET_PUBLIC_KEY = 0x05, /// public key of corresponding BIP32 path
    SIGN_TX = 0x06 /// sign transaction with BIP32 path
} command_e;

typedef enum ApduState {
    UNINITIALIZED = 0,
    PAYLOAD_IN_PROGRESS,
    PAYLOAD_COMPLETED,
} ApduState;

typedef enum ApduReply {
    SDK_EXCEPTION = 0x6801,
    SDK_INVALID_PARAMETER = 0x6802,
    SDK_EXCEPTION_OVERFLOW = 0x6803,
    SDK_EXCEPTION_SECURITY = 0x6804,
    SDK_INVALID_CRC = 0x6805,
    SDK_INVALID_CHECKSUM = 0x6806,
    SDK_INVALID_COUNTER = 0x6807,
    SDK_NOT_SUPPORTED = 0x6808,
    SDK_INVALID_STATE = 0x6809,
    SDK_TIMEOUT = 0x6810,
    SDK_EXCEPTION_PIC = 0x6811,
    SDK_EXCEPTION_APP_EXIT = 0x6812,
    SDK_EXCEPTION_IO_OVERFLOW = 0x6813,
    SDK_EXCEPTION_IO_HEADER = 0x6814,
    SDK_EXCEPTION_IO_STATE = 0x6815,
    SDK_EXCEPTION_IO_RESET = 0x6816,
    SDK_EXCEPTION_CX_PORT = 0x6817,
    SDK_EXCEPTION_SYSTEM = 0x6818,
    SDK_NOT_ENOUGH_SPACE = 0x6819,

    NO_APDU_RECEIVED = 0x6982,
    USER_REFUSAL = 0x6985,

    RENEC_INVALID_MESSAGE = 0x6a80,
    RENEC_INVALID_MESSAGE_HEADER = 0x6a81,
    RENEC_INVALID_MESSAGE_FORMAT = 0x6a82,
    RENEC_INVALID_MESSAGE_SIZE = 0x6a83,
    RENEC_SUMMARY_FINALIZE_FAILED = 0x6f00,
    RENEC_SUMMARY_UPDATE_FAILED = 0x6f01,

    UNIMPLEMENTED_INSTRUCTION = 0x6d00,
    INVALID_CLA = 0x6e00,

    SUCCESS = 0x9000,
} ApduReply;

/**
 * Enumeration with expected INS of APDU commands.
 */
typedef enum {
    INS_GET_APP_CONFIGURATION = 0x01, // app configuration of the application
    INS_GET_PUBLIC_KEY = 0x02, // public key of corresponding BIP32 path
    INS_SIGN_MESSAGE = 0x03, // sign message with BIP32 path
    INS_SIGN_OFFCHAIN_MESSAGE = 0x04, // sign offchain message with BIP32 path
    INS_SIGN_TX = 0x05 /// sign transaction with BIP32 path
} ApduInstructionCode;

/**
 * Structure with fields of APDU command.
 */
typedef struct ApduHeader {
    uint8_t cla; // Instruction class
    uint8_t instruction; // Instruction code
    uint8_t p1; // Instruction parameter 1
    uint8_t p2; // Instruction parameter 2
    const uint8_t* data; // Command data
    size_t data_length; // Length of command data
} ApduHeader;

typedef struct ApduCommand {
    ApduState state;
    ApduInstructionCode instruction;
    uint8_t num_derivation_paths;
    uint32_t derivation_path[MAX_BIP32_PATH_LENGTH];
    uint32_t derivation_path_length;
    bool non_confirm;
    uint8_t message[MAX_MESSAGE_LENGTH];
    int message_length;
    Hash message_hash;
} ApduCommand;

extern ApduCommand G_command;

/**
 * Deserialize APDU into ApduCommand structure.
 *
 * @param[in] apdu_message
 *   Pointer to raw APDU buffer.
 * @param[in] apdu_message_len
 *   Size of the APDU buffer.
 * @param[out] apdu_command
 *   Pointer to ApduCommand structure.
 *
 * @return zero on success, ApduReply error code otherwise.
 *
 */
int apdu_handle_message(const uint8_t* apdu_message,
                        size_t apdu_message_len,
                        ApduCommand* apdu_command);

// typedef enum {
//     INS_SIGN_TX = 0x04,        // sign transaction with BIP32 path
//     INS_GET_VERSION = 0x06,   // instruction get version of the application
// } command_e;

// typedef struct {
//     uint8_t cla;    // Instruction class
//     command_e ins;  // Instruction code
//     uint8_t p1;     // Instruction parameter 1
//     uint8_t p2;     // Instruction parameter 2
//     uint8_t lc;     // Length of command data
//     uint8_t *data;  // Command data
// } command_t;