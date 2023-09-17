// Ledger SDK imports
#include <buffer.h>
#include <decorators.h>
#include <errors.h>
#include <exceptions.h>
#include <os_halt.h>
#include <os_io.h>
#include <os_io_seproxyhal.h>
#include <seproxyhal_protocol.h>

// Local imports
#include "apdu/sw.h"
#include "common/write.h"
#include "globals.h"
#include "io.h"
#include "ui/ui_api.h"

#ifdef HAVE_BAGL
void io_seproxyhal_display(const bagl_element_t *element) {
    io_seproxyhal_display_default((bagl_element_t *) element);
}
#endif // HAVE_BAGL

uint8_t io_event(uint8_t channel __attribute__((unused))) {
    switch (G_io_seproxyhal_spi_buffer[0]) {
#ifdef HAVE_NBGL
        case SEPROXYHAL_TAG_FINGER_EVENT:
            UX_FINGER_EVENT(G_io_seproxyhal_spi_buffer);
            break;
#endif // HAVE_NBGL

#ifdef HAVE_BAGL
        case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
            UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer);
            break;
#endif // HAVE_BAGL

        case SEPROXYHAL_TAG_STATUS_EVENT:
            // TODO
            FALL_THROUGH;

        case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
#ifdef HAVE_BAGL
            UX_DISPLAYED_EVENT({});
#endif // HAVE_BAGL
            break;

        case SEPROXYHAL_TAG_TICKER_EVENT:
            UX_TICKER_EVENT(G_io_seproxyhal_spi_buffer, {});
            break;
        
        default:
            UX_DEFAULT_EVENT();
            break;
    }

    if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
    }

    return 1;
}

uint16_t io_exchange_al(uint8_t channel, uint16_t tx_len) {
    switch (channel & ~(IO_FLAGS)) {
        case CHANNEL_KEYBOARD:
            break;

        case CHANNEL_SPI:
            if (tx_len) {
                io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

                if (channel & IO_RESET_AFTER_REPLIED) {
                    halt();
                }

                return 0;
            } else {
                return io_seproxyhal_spi_recv(G_io_apdu_buffer, sizeof(G_io_apdu_buffer), 0);
            }
        
        default:
            THROW(INVALID_PARAMETER);
    }
}

int io_recv_command() {
    int ret = -1;

    switch (G_io_state) {
        case READY:
            G_io_state = RECEIVED;
            ret = io_exchange(CHANNEL_APDU, G_output_len);
            break;
        case RECEIVED:
            G_io_state = WAITING;
            ret = io_exchange(CHANNEL_APDU | IO_ASYNCH_REPLY, G_output_len);
            G_io_state = RECEIVED;
            break;
        case WAITING:
            G_io_state = READY;
            ret = -1;
            break;
    }

    return ret;
}

int io_send_response(const buffer_t *rdata, uint16_t sw) {
    int ret;

    if (rdata != NULL) {
        if (rdata->size - rdata->offset > IO_APDU_BUFFER_SIZE - 2 || 
            !buffer_copy(rdata, G_io_apdu_buffer, sizeof(G_io_apdu_buffer))) {
            return io_send_sw(SW_WRONG_RESPONSE_LENGTH);
        }
        G_output_len = rdata->size - rdata->offset;
        PRINTF("<= SW=%04X | RData=%.*H\n", sw, rdata->size, rdata->ptr);
    } else {
        G_output_len = 0;
        PRINTF("<= SW=%04X | RData=\n", sw);
    }

    write_u16_be(G_io_apdu_buffer, G_output_len, sw);
    G_output_len += 2;

    // If we are in swap mode and have validated a TX, we send it and immediately quit
    // TODO Implement swap flow
    // if (G_called_from_swap && G.)

    switch (G_io_state) {
        case READY:
            PRINTF("G_io_state error\n");
            ret = -1;
            break;
        case RECEIVED:
            G_io_state = READY;
            ret = 0;
            break;
        case WAITING:
            ret = io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, G_output_len);
            G_output_len = 0;
            G_io_state = READY;
            break;
    }

    return ret;
}

int io_send_sw(uint16_t sw) {
    return io_send_response(NULL, sw);
}

void sendResponse(uint8_t tx, uint16_t sw, bool display_menu) {
    // Write status word
    write_u16_be(G_io_apdu_buffer, tx, sw);
    tx += 2;

    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);

    // TODO Handle swap

    if (display_menu) {
        // Display back the original UX
        ui_idle();
    }
}