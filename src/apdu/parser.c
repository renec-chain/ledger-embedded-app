#include "constants.h"
#include "parser.h"

bool apdu_parser(command_t *cmd, uint8_t *buf, size_t buf_len) {
    // Check minimum length and Lc field of APDU command
    if (buf_len < OFFSET_CDATA || buf_len - OFFSET_CDATA != buf[OFFSET_LC]) {
        return false;
    }

    cmd->cla = buf[OFFSET_CLA];
    cmd->ins = (command_e) buf[OFFSET_INS];
    cmd->p1 = buf[OFFSET_P1];
    cmd->p2 = buf[OFFSET_P2];
    cmd->lc = buf[OFFSET_LC];
    cmd->data = (buf[OFFSET_LC] > 0) ? buf + OFFSET_CDATA : NULL;

    return true;
}