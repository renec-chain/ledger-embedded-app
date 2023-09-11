#pragma once

/**
 * Length of APPNAME variable in Makefile.
 */
#define APPNAME_LENGTH (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION.
 */
#define APP_VERSION_LENGTH 3

/**
 * Length of hash_signing_enabled
 */
#define APP_CONFIGURATION_LENGTH 1

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LENGTH 64

/**
 * Maximum transaction length (bytes).
 */
#define MAX_TRANSACTION_LENGTH 510

#define ROUND_TO_NEXT(x, next) (((x) == 0) ? 0 : ((((x - 1) / (next)) + 1) * (next)))

#define PACKAGE_DATA_SIZE (1280 - 40 - 8)

/**
 * Maximum bip32 path length (bytes).
 */
#define MAX_BIP32_PATH_LENGTH 5

#define MAX_DERIVATION_PATH_BUFFER_LENGTH (1 + MAX_BIP32_PATH_LENGTH * 4)
#define TOTAL_SIGN_MESSAGE_BUFFER_LENGTH (PACKAGE_DATA_SIZE + MAX_DERIVATION_PATH_BUFFER_LENGTH)

#define MAX_MESSAGE_LENGTH ROUND_TO_NEXT(TOTAL_SIGN_MESSAGE_BUFFER_LENGTH, USB_SEGMENT_SIZE)

/**
 * Maximum signature length (bytes).
 */
#define MAX_DER_SIG_LEN 72

/**
 * Signature length (bytes).
 */
#define SIGNATURE_LENGTH 64

/**
 * Exponent used to convert mBOL to BOL unit (N BOL = N * 10^3 mBOL).
*/
#define EXPONENT_SMALLEST_UNIT 3