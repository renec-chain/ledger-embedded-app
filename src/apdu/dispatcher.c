#include "constants.h"
#include "dispatcher.h"
#include "sw.h"

#include "../handler/get_version.h"

#include "../io.h"

int apdu_dispatcher(const command_t *cmd) {
    if (cmd->cla != CLA) {
        return io_send_sw(SW_CLA_NOT_SUPPORTED);
    }

    // TODO Make use of this when adding more APDU commands
    // buffer_t buf = {0};

    switch (cmd->ins) {
        case INS_GET_VERSION:
            if (cmd->p1 != 0 || cmd->p2 != 0) {
                return io_send_sw(SW_WRONG_P1P2);
            }
            return handler_get_version();
        default:
            return io_send_sw(SW_INS_NOT_SUPPORTED);
    }
}