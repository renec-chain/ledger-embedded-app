#pragma once

#include <stdbool.h>

#include "../constants.h"

#include "../common/buffer.h"

extern char G_publicKeyStr[BASE58_PUBKEY_LENGTH];

void reset_getpubkey_globals(void);

uint8_t set_result_get_pubkey(void);

/**
 * Handler INS_GET_VERSION command. Send APDU response with version
 * of the application.
 * 
 * @see MAJOR_VERSION, MINOR_VERSION and PATCH_VERSION in Makefile.
 * 
 * @return zero or positive integer if success, negative integer otherwise.
 * 
 */
// int handler_get_version(void);

void handle_get_pubkey(volatile unsigned int *flags, volatile unsigned int *tx);


/**
 * Handler for INS_SIGN_TX command. If successfully parse BIP32 path
 * and transaction, sign transaction and send APDU response.
 *
 * @param[in,out] cdata
 *   Command data with BIP32 path and raw transaction serialized.
 * @param[in]     is_first_chunk
 *   Is the first data chunk
 * @param[in]       more
 *   Whether more APDU chunk to be received or not.
 *
 * @return zero or positive integer if success, negative integer otherwise.
 *
 */
// int handler_sign_tx(buffer_t *cdata, bool is_first_chunk, bool more);
