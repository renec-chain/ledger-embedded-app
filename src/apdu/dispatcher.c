#include "exceptions.h"
#include "constants.h"
#include "dispatcher.h"
#include "sw.h"

#include "../common/buffer.h"

#include "../handler/handler.h"

#include "../io.h"

#include "types.h"

#include "../utils.h"

int apdu_handle_message(const uint8_t* apdu_message,
                        size_t apdu_message_len,
                        ApduCommand* apdu_command) {
    if (!apdu_command || !apdu_message) {
        return SDK_INVALID_PARAMETER;
    }

    // parse header
    ApduHeader header = {0};

    // must at least hold the class and instruction
    if (apdu_message_len <= OFFSET_INS) {
        return RENEC_INVALID_MESSAGE_SIZE;
    }

    header.cla = apdu_message[OFFSET_CLA];
    if (header.cla != CLA) {
        return RENEC_INVALID_MESSAGE_HEADER;
    }

    header.instruction = apdu_message[OFFSET_INS];
    switch (header.instruction) {
    case INS_GET_APP_CONFIGURATION:
    case INS_GET_PUBLIC_KEY:
    case INS_SIGN_MESSAGE:
    case INS_SIGN_OFFCHAIN_MESSAGE: {
        // must at least hold a full modern header
        if (apdu_message_len < OFFSET_CDATA) {
            return RENEC_INVALID_MESSAGE_SIZE;
        }

        // modern data maybe up to 255B
        if (apdu_message_len > UINT8_MAX + OFFSET_CDATA) {
            return RENEC_INVALID_MESSAGE_SIZE;
        }

        header.data_length = apdu_message[OFFSET_LC];
        if (apdu_message_len != header.data_length + OFFSET_CDATA) {
            return RENEC_INVALID_MESSAGE_SIZE;
        }

        if (header.data_length > 0) {
            header.data = apdu_message + OFFSET_CDATA;
        }

        break;
    }
    default:
        return UNIMPLEMENTED_INSTRUCTION;
    }

    header.p1 = apdu_message[OFFSET_P1];
    header.p2 = apdu_message[OFFSET_P2];
    // P2_EXTEND is set to signal that this APDU buffer extends, rather
    // than replaces, the current message buffer
    const bool first_data_chunk = !(header.p2 & P2_EXTEND);

    if (header.instruction == INS_GET_APP_CONFIGURATION) {
        // return early if no data is expected for the command
        explicit_bzero(apdu_command, sizeof(ApduCommand));
        apdu_command->state = PAYLOAD_COMPLETED;
        apdu_command->instruction = header.instruction;
        apdu_command->non_confirm = (header.p1 == P1_NON_CONFIRM);

        return 0;
    } else if (header.instruction == INS_SIGN_MESSAGE ||
               header.instruction == INS_SIGN_OFFCHAIN_MESSAGE) {
        if (!first_data_chunk) {
            // validate the command in progress
            if (apdu_command->state != PAYLOAD_IN_PROGRESS ||
                apdu_command->instruction != header.instruction ||
                apdu_command->non_confirm != (header.p1 == P1_NON_CONFIRM) ||
                apdu_command->num_derivation_paths != 1) {
                return RENEC_INVALID_MESSAGE;
            }
        } else {
            explicit_bzero(apdu_command, sizeof(ApduCommand));
        }
    } else {
        explicit_bzero(apdu_command, sizeof(ApduCommand));
    }

    // read derivation path
    if (first_data_chunk) {
        if (!header.instruction != INS_GET_PUBLIC_KEY) {
            if (!header.data_length) {
                return RENEC_INVALID_MESSAGE_SIZE;
            }
            apdu_command->num_derivation_paths = header.data[0];
            header.data++;
            header.data_length--;
            // We only support one derivation path at the moment
            if (apdu_command->num_derivation_paths != 1) {
                return RENEC_INVALID_MESSAGE;
            }
        } else {
            apdu_command->num_derivation_paths = 1;
        }
        const int ret = read_derivation_path(header.data,
                                             header.data_length,
                                             apdu_command->derivation_path,
                                             apdu_command->derivation_path_length);
        if (ret) {
            return ret;
        }
        header.data += 1 + apdu_command->derivation_path_length * 4;
        header.data_length -= 1 + apdu_command->derivation_path_length * 4;
    }

    apdu_command->state = PAYLOAD_IN_PROGRESS;
    apdu_command->instruction = header.instruction;
    apdu_command->non_confirm = (header.p1 == P1_NON_CONFIRM);

    if (header.data) {
        if (apdu_command->message_length + header.data_length > MAX_MESSAGE_LENGTH) {
            return RENEC_INVALID_MESSAGE_SIZE;
        }

        memcpy(apdu_command->message + apdu_command->message_length,
               header.data,
               header.data_length);
        apdu_command->message_length += header.data_length;
    } else if (header.instruction != INS_GET_PUBLIC_KEY) {
        return RENEC_INVALID_MESSAGE_SIZE;
    }

    // check if more data is expected
    if (header.p2 & P2_MORE) {
        return 0;
    }

    apdu_command->state = PAYLOAD_COMPLETED;

    return 0;
}

// int apdu_dispatcher(const command_t *cmd) {
//     if (cmd->cla != CLA) {
//         return io_send_sw(SW_CLA_NOT_SUPPORTED);
//     }

//     buffer_t buf = {0};

//     switch (cmd->ins) {
//         case INS_GET_VERSION:
//             if (cmd->p1 != 0 || cmd->p2 != 0) {
//                 return io_send_sw(SW_WRONG_P1P2);
//             }
//             return handler_get_version();
//         case INS_SIGN_TX:
//             if ((cmd->p1 != P1_FIRST && cmd->p1 != P1_MORE) ||
//                 (cmd->p2 != P2_LAST && cmd->p2 != P2_MORE)) {
//                     return io_send_sw(SW_WRONG_P1P2);
//                 }
            
//             if (!cmd->data) {
//                 return io_send_sw(SW_WRONG_DATA_LENGTH);
//             }

//             buf.ptr = cmd->data;
//             buf.size = cmd->lc;
//             buf.offset = 0;

//             return handler_sign_tx(&buf, !cmd->p1, (bool) (cmd->p2 & P2_MORE));
//         default:
//             return io_send_sw(SW_INS_NOT_SUPPORTED);
//     }
// }