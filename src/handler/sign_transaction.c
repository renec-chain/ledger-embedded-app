#include <string.h>

#include "handler.h"
#include "globals.h"

#include "../apdu/sw.h"

#include "../common/crypto.h"
#include "../common/cx.h"

// int handler_sign_tx(buffer_t *cdata, bool is_first_chunk, bool more) {
//     if (is_first_chunk) {
//         explicit_bzero(&G_context, sizeof(G_context));
//     }

//     if (G_context.tx_info.raw_size + cdata->size > RAW_TX_MAX_SIZE) {
//         return io_send_sw(SW_WRONG_T569X_LENGTH);
//     }

//     if (is_first_chunk) {
//         G_context.req_type = CONFIRM_TRANSACTION;
//         G_context.state = STATE_NONE;

//         if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
//             !buffer_read_bip32_path(cdata,
//                                     G_context.bip32_path,
//                                     (size_t) G_context.bip32_path_len)) {
//             return io_send_sw(SW_WRONG_DATA_LENGTH);
//         }

//         size_t data_length = cdata->size - cdata->offset;
//         memcpy(G_context.tx_info.raw, cdata->ptr + cdata->offset, data_length);
//         G_context.tx_info.raw_size += data_length;
//     } else {
//         if (G_context.req_type != CONFIRM_TRANSACTION) {
//             return io_send_sw(SW_BAD_STATE);
//         }
//         memcpy(G_context.tx_info.raw + G_context.tx_info.raw_size, cdata->ptr, cdata->size);
//         G_context.tx_info.raw_size += cdata->size;
//     }

//     PRINTF("data size: %d\n", G_context.tx_info.raw_size);

//     if (more) {
//         return io_send_sw(SW_OK);
//     }

//     // if (!parse_tx_xdr())

//     G_context.state = STATE_PARSED;
//     PRINTF("tx parsed.\n");

//     if (G_called_from_swap) {
//         // TODO Handle swap tx
//     } else {
//         // Normal mode, derive public_key and display the validation UI
//         cx_ecfp_private_key_t private_key = {0};
//         cx_ecfp_public_key_t public_key = {0};

//         // derive private key according to BIP32 path
//         int error =
//             crypto_derive_private_key(&private_key, G_context.bip32_path, G_context.bip32_path_len);
//         if (error != 0) {
//             explicit_bzero(&private_key, sizeof(private_key));
//             return io_send_sw(error);
//         }

//         // generate corresponding public key
//         crypto_init_public_key(&private_key, &public_key, G_context.raw_public_key);
//         // reset private key
//         explicit_bzero(&private_key, sizeof(private_key));

//         if (cx_hash_sha256(G_context.tx_info.raw,
//                            G_context.tx_info.raw_size,
//                            G_context.hash,
//                            HASH_SIZE) != HASH_SIZE) {
//             THROW(SW_TX_HASH_FAIL);
//         }

//         return ui_approve_tx_init();
//     }
// }
