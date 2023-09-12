#include <os.h>
#include <os_io_seproxyhal.h>
#include <ux.h>

#include "apdu/dispatcher.h"
#include "apdu/parser.h"
#include "apdu/sw.h"
#include "apdu/types.h"
#include "ui/ui.h"

#include "globals.h"
#include "io.h"
#include "settings.h"

// The settings, stored in NVRAM.
const internal_storage_t N_storage_real;

/**
 * Handle APDU command received and send back APDU response using handlers.
 */
void app_main() {
    // Length of APDU command received
    int input_len = 0;
    // Structured APDU command
    command_t cmd;

    // Reset length of APDU response
    G_output_len = 0;
    G_io_state = READY;

    // Reset context
    explicit_bzero(&G_context, sizeof(G_context));

    for (;;) {
        BEGIN_TRY {
            TRY {
                // Reset structured APDU command
                memset(&cmd, 0, sizeof(cmd));

                // Receive command bytes in G_io_apdu_buffer
                if ((input_len == io_recv_command()) != 0) {
                    return;
                }

                // Parse APDU command from G_io_apdu_buffer
                if (!apdu_parser(&cmd, G_io_apdu_buffer, input_len)) {
                    PRINTF("=> /!\\ BAD LENGTH: %.*H\n", input_len, G_io_apdu_buffer);
                    io_send_sw(SW_WRONG_RESPONSE_LENGTH);
                    continue;
                }

                PRINTF("=> CLA=%02X | INS=%02X | P1=%02X | P2=%02X | Lc=%02X | Cdata=%.*H\n",
                       cmd.cla,
                       cmd.ins,
                       cmd.p1,
                       cmd.p2,
                       cmd.lc,
                       cmd.data);

                // Dispatch structured APDU command to handler
                if (apdu_dispatcher(&cmd) > 0) {
                    return;
                }
            }
            // TODO EXCEPTION_IO_RESET is legacy value, check the latest ledger-secured-sdk for handling latest one
            CATCH(EXCEPTION_IO_RESET) {
                THROW(EXCEPTION_IO_RESET);
            }
            CATCH_OTHER(e) {
                io_send_sw(e);
            }
            FINALLY {
            }
            END_TRY;
        }
    }
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

void standalone_app_main(void) {
    G_called_from_swap = false;
    for (;;) {
        // TODO init ux
//         UX_INIT();

        // Reset UI
        memset(&G_ux, 0, sizeof(G_ux));

        BEGIN_TRY {
            TRY {
                io_seproxyhal_init();

                // nv_app_state_init();
                if (!HAS_SETTING(S_INITIALIZED)) {
                    internal_storage_t storage = 0x00;
                    storage |= 0x80;
                    nvm_write((void *) &N_settings, (void *) &storage, sizeof(internal_storage_t));
                }

#ifdef HAVE_BLE
                // Grab the current plane mode setting. os_setting_get() is enabled by
                // appFlags bit #9 set to 1 in Makefile (i.e. "--appFlags 0x2xxx")
                G_io_app.plane_mode = os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
#endif  // HAVE_BLE TARGET_NANOX

                USB_power(0);
                USB_power(1);

                // TODO Implement this
                ui_menu_main();

                // ui_idle();

#ifdef HAVE_BLE
                BLE_power(0, NULL);
                BLE_power(1, "Nano X");
#endif  // HAVE_BLE

                app_main();
            }
            CATCH(EXCEPTION_IO_RESET) {
                // reset IO and UI before continuing
                CLOSE_TRY;
                continue;
            }
            CATCH_ALL {
                CLOSE_TRY;
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
 * Main loop to setup USB, Bluetooth, UI and launch ui_menu_main().
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
        app_exit();
    }

    return 0;
}