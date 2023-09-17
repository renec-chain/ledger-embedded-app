#include <stdint.h>
#include <limits.h>
#include <assert.h>

#include <os.h>
#include "io.h"

#include "handler.h"

#include "../apdu/sw.h"

#include "../common/buffer.h"

#include "../constants.h"
#include "../settings.h"

// int handler_get_version() {
//     PRINTF("handler_get_version invoked\n");

//     _Static_assert(APP_VERSION_LENGTH == 3, "Length of (MAJOR || MINOR || PATCH) must be 3!");
//     _Static_assert(MAJOR_VERSION >= 0 && MAJOR_VERSION <= UINT8_MAX,
//                    "MAJOR version must be between 0 and 255!");
//     _Static_assert(MINOR_VERSION >= 0 && MINOR_VERSION <= UINT8_MAX,
//                    "MINOR version must be between 0 and 255!");
//     _Static_assert(PATCH_VERSION >= 0 && PATCH_VERSION <= UINT8_MAX,
//                    "PATCH version must be between 0 and 255!");

//     return io_send_response(
//         &(const buffer_t){.ptr = 
//                             (uint8_t[APP_CONFIGURATION_LENGTH + APP_VERSION_LENGTH]){
//                                 (uint8_t) HAS_SETTING(S_HASH_SIGNING_ENABLED),
//                                 (uint8_t) MAJOR_VERSION,
//                                 (uint8_t) MINOR_VERSION,
//                                 (uint8_t) PATCH_VERSION},
//                           .size = APP_CONFIGURATION_LENGTH + APP_VERSION_LENGTH,
//                           .offset = 0},
//         SW_OK
//     );

//     // return io_send_response_pointer(
//     //     (const uint8_t *) &(uint8_t[APP_VERSION_LENGTH]){(uint8_t) MAJOR_VERSION,
//     //                                                      (uint8_t) MINOR_VERSION,
//     //                                                      (uint8_t) PATCH_VERSION},
//     //     APP_VERSION_LENGTH,
//     //     StatusWord.SUCCESS
//     // );
// }