#pragma once

#include "renec/print_config.h"

struct Instruction;
struct MessageHeader;
struct Pubkey;

extern const Pubkey rpl_associated_token_account_program_id;

typedef struct RplAssociatedTokenAccountCreateInfo {
    const Pubkey* funder;
    const Pubkey* address;
    const Pubkey* owner;
    const Pubkey* mint;
} RplAssociatedTokenAccountCreateInfo;

typedef struct RplAssociatedTokenAccountInfo {
    union {
        RplAssociatedTokenAccountCreateInfo create;
    };
} RplAssociatedTokenAccountInfo;

int parse_rpl_associated_token_account_instructions(const Instruction* instruction,
                                                    const MessageHeader* header,
                                                    RplAssociatedTokenAccountInfo* info);

int print_rpl_associated_token_account_info(const RplAssociatedTokenAccountInfo* info,
                                            const PrintConfig* print_config);

int print_rpl_associated_token_account_create_info(const RplAssociatedTokenAccountCreateInfo* info,
                                                   const PrintConfig* print_config);
