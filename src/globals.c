#include "globals.h"

io_state_e G_io_state;
uint32_t G_output_len;
ux_state_t G_ux;
// uint8_t G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

global_ctx_t G_context;
bolos_ux_params_t G_ux_params;

bool G_called_from_swap;
const internalStorage_t N_storage_real;