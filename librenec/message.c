#include "instruction.h"
#include "renec/parser.h"
#include "renec/message.h"
#include "renec/print_config.h"
#include "rpl_associated_token_account_instruction.h"
#include "rpl_token_instruction.h"
#include "system_instruction.h"
#include "stake_instruction.h"
#include "vote_instruction.h"
#include "transaction_printers.h"
#include "util.h"
#include <string.h>

#define MAX_INSTRUCTIONS 4

int process_message_body(const uint8_t* message_body,
                         int message_body_length,
                         const PrintConfig* print_config) {
    const MessageHeader* header = &print_config->header;

    BAIL_IF(header->instructions_length == 0);
    BAIL_IF(header->instructions_length > MAX_INSTRUCTIONS);

    size_t instruction_count = 0;
    InstructionInfo instruction_info[MAX_INSTRUCTIONS];
    explicit_bzero(instruction_info, sizeof(InstructionInfo) * MAX_INSTRUCTIONS);

    size_t display_instruction_count = 0;
    InstructionInfo* display_instruction_info[MAX_INSTRUCTIONS];

    Parser parser = {message_body, message_body_length};
    for (; instruction_count < header->instructions_length; instruction_count++) {
        Instruction instruction;
        BAIL_IF(parse_instruction(&parser, &instruction));
        BAIL_IF(instruction_validate(&instruction, header));

        InstructionInfo* info = &instruction_info[instruction_count];
        enum ProgramId program_id = instruction_program_id(&instruction, header);
        switch (program_id) {
            case ProgramIdRplAssociatedTokenAccount: {
                if (parse_rpl_associated_token_account_instructions(
                        &instruction,
                        header,
                        &info->rpl_associated_token_account) == 0) {
                    info->kind = program_id;
                }
                break;
            }
            case ProgramIdRplMemo: {
                // RPL Memo only has one instruction and we ignore it for now
                info->kind = program_id;
                break;
            }
            case ProgramIdRplToken:
                if (parse_rpl_token_instructions(&instruction, header, &info->rpl_token) == 0) {
                    info->kind = program_id;
                }
                break;
            case ProgramIdSystem: {
                if (parse_system_instructions(&instruction, header, &info->system) == 0) {
                    info->kind = program_id;
                }
                break;
            }
            case ProgramIdStake: {
                if (parse_stake_instructions(&instruction, header, &info->stake) == 0) {
                    info->kind = program_id;
                }
                break;
            }
            case ProgramIdVote: {
                if (parse_vote_instructions(&instruction, header, &info->vote) == 0) {
                    info->kind = program_id;
                }
                break;
            }
            case ProgramIdUnknown:
                break;
        }
        switch (info->kind) {
            case ProgramIdRplAssociatedTokenAccount:
            case ProgramIdRplToken:
            case ProgramIdSystem:
            case ProgramIdStake:
            case ProgramIdVote:
            case ProgramIdUnknown:
                display_instruction_info[display_instruction_count++] = info;
                break;
            // Ignored instructions
            case ProgramIdRplMemo:
                break;
        }
    }

    if (header->versioned) {
        size_t account_tables_length;
        BAIL_IF(parse_length(&parser, &account_tables_length));
        BAIL_IF(account_tables_length > 0);
    }

    // Ensure we've consumed the entire message body
    BAIL_IF(!parser_is_empty(&parser));

    // If we don't know about all of the instructions, bail
    for (size_t i = 0; i < instruction_count; i++) {
        BAIL_IF(instruction_info[i].kind == ProgramIdUnknown);
    }

    return print_transaction(print_config, display_instruction_info, display_instruction_count);
}
