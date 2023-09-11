#pragma once

/**
 * Instruction class of RENEC application.
 */
#define CLA 0xE0

#define P1_CONFIRM 0x01
#define P1_NON_CONFIRM 0x00

#define P2_EXTEND 0x01
#define P2_MORE 0x02

// header offsets
#define OFFSET_CLA 0
#define OFFSET_INS 1
#define OFFSET_P1 2
#define OFFSET_P2 3
#define OFFSET_LC 4
#define OFFSET_CDATA 5