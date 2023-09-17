#pragma once

// Standard libraries imports
#include <stdbool.h>
#include <stdint.h>

// Ledger SDK imports
#include <ux.h>
#include <io.h>
#include <os.h>
#include <os_pic.h>
#include <os_io_seproxyhal.h>

#include "io.h"
#include "types.h"

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

/**
 * 
 * Global variable with the length of APDU response to send back.
 * 
 */
extern uint32_t G_output_len;

/**
 * 
 * Global enumeration with the state of IO (READY, RECEIVED, WAITING).
 * 
 */
extern io_state_e G_io_state;

/**
 * 
 * Global context for user requests.
 * 
 */
extern global_ctx_t G_context;

/**
 * 
 * It is called through the exchange app
 * 
 */
extern bool G_called_from_swap;

/**
 * Global buffer for interactions between SE and MCU.
 */
// extern uint8_t G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

/**
 * 
 * Use an union to avoid the UI variable footprints for the swap flow and vice versa
 * 
 */


/**
 * Global structure with the parameters to exchange with the BOLOS UX application.
 */
extern bolos_ux_params_t G_ux_params;

enum BlindSign {
    BLIND_SIGN_DISABLED = 0,
    BLIND_SIGN_ENABLED = 1,
};

enum PubkeyDisplay {
    PUBKEY_DISPLAY_LONG = 0,
    PUBKEY_DISPLAY_SHORT = 1,
};

enum DisplayMode {
    DISPLAY_MODE_USER = 0,
    DISPLAY_MODE_EXPERT = 1,
};

typedef struct AppSettings {
    uint8_t allow_blind_sign;
    uint8_t pubkey_display;
    uint8_t display_mode;
} AppSettings;

typedef struct internalStorage_t {
    AppSettings settings;
    uint8_t initialized;
} internalStorage_t;

extern const internalStorage_t N_storage_real;
#define N_storage (*(volatile internalStorage_t*) PIC(&N_storage_real))
#endif
