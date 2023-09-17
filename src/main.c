#include <os.h>
#include <os_io_seproxyhal.h>
#include <ux.h>

#include "apdu/dispatcher.h"
#include "apdu/parser.h"
#include "apdu/sw.h"
#include "apdu/types.h"
#include "ui/ui.h"

#include "common/utils.h"

#include "handler/handler.h"

#include "globals.h"
#include "io.h"
#include "settings.h"

// The settings, stored in NVRAM.
// const internal_storage_t N_storage_real;

ApduCommand G_command;
unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

static void reset_main_globals(void) {
    MEMCLEAR(G_command);
    MEMCLEAR(G_io_seproxyhal_spi_buffer);
}

void handle_apdu(volatile unsigned int *flags, volatile unsigned int *tx, int rx) {
    if (!flags || !tx) {
        THROW(SDK_INVALID_PARAMETER);
    }
    if (rx < 0) {
        THROW(SDK_EXCEPTION_IO_OVERFLOW);
    }

    const int ret = apdu_handle_message(G_io_apdu_buffer, rx, &G_command);
    if (ret != 0) {
        MEMCLEAR(G_command);
        THROW(ret);
    }

    if (G_command.state == PAYLOAD_IN_PROGRESS) {
        THROW(SUCCESS);
    }

    switch (G_command.instruction) {
    case INS_GET_APP_CONFIGURATION:
        G_io_apdu_buffer[0] = N_storage.settings.allow_blind_sign;
        G_io_apdu_buffer[1] = N_storage.settings.pubkey_display;
        G_io_apdu_buffer[2] = MAJOR_VERSION;
        G_io_apdu_buffer[3] = MINOR_VERSION;
        G_io_apdu_buffer[4] = PATCH_VERSION;
        *tx = 5;
        THROW(SUCCESS);
    case INS_GET_PUBLIC_KEY:
        handle_get_pubkey(flags, tx);
        break;
    case INS_SIGN_MESSAGE:
        // handle_sign_message_parse_message(tx);
        // handle_sign_message_ui(flags);
        // break;
        THROW(UNIMPLEMENTED_INSTRUCTION);
    default:
        THROW(UNIMPLEMENTED_INSTRUCTION);
    }
}

/**
 * Handle APDU command received and send back APDU response using handlers.
 */
void app_main() {
    volatile unsigned int rx = 0;
    volatile unsigned int tx = 0;
    volatile unsigned int flags = 0;

    // Store the information about the current command. Some commands expect
    // multiple APDUs before they become complete and executed.
    reset_getpubkey_globals();
    reset_main_globals();

    // DESIGN NOTE: the bootloader ignores the way APDU are fetched. The only
    // goal is to retrieve APDU.
    // When APDU are to be fetched from multiple IOs, like NFC + USB + BLE, make
    // sure the io_event is called with a
    // switch event, before the apdu is replied to the bootloader. This avoids
    // APDU injection faults.
    for (;;) {
        volatile unsigned short sw = 0;
        BEGIN_TRY {
            TRY {
                rx = tx;
                tx = 0; // ensure no race in catch_other if io_exchange throws an error
                rx = io_exchange(CHANNEL_APDU | flags, rx);
                flags = 0;

                // no APDU received, reset the session, and reset the bootloader configuration
                if (rx == 0) {
                    THROW(NO_APDU_RECEIVED);
                }

                PRINTF("New APDU received:\n%.*H\n", rx, G_io_apdu_buffer);
                handle_apdu(&flags, &tx, rx);
            }
            CATCH(SDK_EXCEPTION_IO_RESET) {
                THROW(SDK_EXCEPTION_IO_RESET);
            }
            CATCH_OTHER(e) {
                switch (e & 0xF000) {
                case 0x6000:
                    sw = e;
                    break;
                case 0x9000:
                    // All is well
                    sw = e;
                    break;
                default:
                    // Internal error
                    sw = 0x6800 | (e & 0x7FF);
                    break;
                }
                if (e != 0x9000) {
                    flags &= ~IO_ASYNCH_REPLY;
                }
                // Unexpected exception => report
                G_io_apdu_buffer[tx] = sw >> 8;
                G_io_apdu_buffer[tx + 1] = sw;
                tx += 2;
            }
            FINALLY {
            }
        }
        END_TRY;
    }

    // // Length of APDU command received
    // int input_len = 0;
    // // Structured APDU command
    // command_t cmd;

    // // Reset length of APDU response
    // G_output_len = 0;
    // G_io_state = READY;

    // // Reset context
    // explicit_bzero(&G_context, sizeof(G_context));

    // for (;;) {
    //     BEGIN_TRY {
    //         TRY {
    //             // Reset structured APDU command
    //             memset(&cmd, 0, sizeof(cmd));

    //             // Receive command bytes in G_io_apdu_buffer
    //             if ((input_len == io_recv_command()) != 0) {
    //                 return;
    //             }

    //             // Parse APDU command from G_io_apdu_buffer
    //             if (!apdu_parser(&cmd, G_io_apdu_buffer, input_len)) {
    //                 PRINTF("=> /!\\ BAD LENGTH: %.*H\n", input_len, G_io_apdu_buffer);
    //                 io_send_sw(SW_WRONG_RESPONSE_LENGTH);
    //                 continue;
    //             }

    //             PRINTF("=> CLA=%02X | INS=%02X | P1=%02X | P2=%02X | Lc=%02X | Cdata=%.*H\n",
    //                    cmd.cla,
    //                    cmd.ins,
    //                    cmd.p1,
    //                    cmd.p2,
    //                    cmd.lc,
    //                    cmd.data);

    //             // Dispatch structured APDU command to handler
    //             if (apdu_dispatcher(&cmd) > 0) {
    //                 return;
    //             }
    //         }
    //         // TODO EXCEPTION_IO_RESET is legacy value, check the latest ledger-secured-sdk for handling latest one
    //         CATCH(EXCEPTION_IO_RESET) {
    //             THROW(EXCEPTION_IO_RESET);
    //         }
    //         CATCH_OTHER(e) {
    //             io_send_sw(e);
    //         }
    //         FINALLY {
    //         }
    //         END_TRY;
    //     }
    // }
}

/**
 * 
 * Exit the application and go back to the dashboard.
 * 
 */
void app_exit(void) {
    BEGIN_TRY_L(exit) {
        TRY_L(exit) {
            os_sched_exit(-1);
        }
        FINALLY_L(exit) {
        }
    }
    END_TRY_L(exit);
}

void nv_app_state_init() {
    if (N_storage.initialized != 0x01) {
        internalStorage_t storage;
        storage.settings.allow_blind_sign = BLIND_SIGN_DISABLED;
#if defined(TARGET_NANOX) || defined(TARGET_NANOS2) || defined(TARGET_STAX)
        storage.settings.pubkey_display = PUBKEY_DISPLAY_LONG;
#else
        storage.settings.pubkey_display = PUBKEY_DISPLAY_SHORT;
#endif
        storage.settings.display_mode = DISPLAY_MODE_USER;
        storage.initialized = 0x01;
        nvm_write((void *) &N_storage, (void *) &storage, sizeof(internalStorage_t));
    }
}

void standalone_app_main(void) {
    G_called_from_swap = false;
    for (;;) {
        UX_INIT();

        BEGIN_TRY {
            TRY {
                io_seproxyhal_init();

                nv_app_state_init();

                USB_power(0);
                USB_power(1);

#ifdef HAVE_BLE
                // Grab the current plane mode setting. os_setting_get() is enabled by
                // appFlags bit #9 set to 1 in Makefile (i.e. "--appFlags 0x2xxx")
                G_io_app.plane_mode = os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
#endif // HAVE_BLE TARGET_NANOX

                ui_idle();

#ifdef HAVE_BLE
                BLE_power(0, NULL);
                BLE_power(1, "Nano X");
#endif // HAVE_BLE

                app_main();
            }
            CATCH(SDK_EXCEPTION_IO_RESET) {
                // reset IO and UI before continuing
                continue;
            }
            CATCH_ALL {
                break;
            }
            FINALLY {
            }
        }
        END_TRY;
    }
    app_exit();
}

/**
 * 
 * Main loop to setup USB, Bluetooth, UI and launch ui_idle().
 * 
 */
__attribute__((section(".boot"))) int main(int arg0) {
    // exit critical section
    __asm volatile("cpsie i");

    // ensure exception will work as planned
    os_boot();

    if (arg0 == 0) {
        // called from dashboard as standalone app
        standalone_app_main();
    } else {
        // TODO called as library from another app
        app_exit();
    }

    return 0;
}