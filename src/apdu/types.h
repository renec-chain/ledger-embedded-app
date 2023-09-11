#pragma once

#include <stddef.h>
#include <stdint.h> // uint*_t

// #include "constants.h"

// #include "../constants.h"
// #include "../renec.h"

// typedef enum {
//     GET_VERSION = 0x03, /// version of the application
//     GET_APP_NAME = 0x04, /// name of the application
//     GET_PUBLIC_KEY = 0x05, /// public key of corresponding BIP32 path
//     SIGN_TX = 0x06 /// sign transaction with BIP32 path
// } command_e;

// typedef enum {
//     UNINITIALIZED = 0,
//     PAYLOAD_IN_PROGRESS,
//     PAYLOAD_COMPLETED,
// } ApduState;

// /**
//  * Enumeration with expected INS of APDU commands.
//  */
// typedef enum {
//     INS_GET_APP_CONFIGURATION = 0x01, // app configuration of the application
//     INS_GET_PUBLIC_KEY = 0x02, // public key of corresponding BIP32 path
//     INS_SIGN_MESSAGE = 0x03, // sign message with BIP32 path
//     INS_SIGN_OFFCHAIN_MESSAGE = 0x04, // sign offchain message with BIP32 path
//     INS_SIGN_TX = 0x05 /// sign transaction with BIP32 path
// } ApduInstructionCode;

// /**
//  * Structure with fields of APDU command.
//  */
// typedef struct {
//     uint8_t cla; // Instruction class
//     ApduInstructionCode instruction; // Instruction code
//     uint8_t p1; // Instruction parameter 1
//     uint8_t p2; // Instruction parameter 2
//     const uint8_t* data; // Command data
//     size_t data_length; // Length of command data
// } ApduHeader;

// typedef struct {
//     ApduState state;
//     ApduInstructionCode instruction;
//     uint8_t num_derivation_paths;
//     uint32_t derivation_path[MAX_BIP32_PATH_LENGTH];
//     uint32_t derivation_path_length;
//     bool non_confirm;
//     uint8_t message[MAX_MESSAGE_LENGTH];
//     int message_length;
//     Hash message_hash;
// } ApduCommand;

// extern ApduCommand G_command;

typedef enum {
    INS_GET_VERSION = 0x06,   // instruction get version of the application
} command_e;

typedef struct {
    uint8_t cla;    // Instruction class
    command_e ins;  // Instruction code
    uint8_t p1;     // Instruction parameter 1
    uint8_t p2;     // Instruction parameter 2
    uint8_t lc;     // Length of command data
    uint8_t *data;  // Command data
} command_t;