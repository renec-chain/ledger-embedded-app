#pragma once

#include "renec/parser.h"
#include "rpl_associated_token_account_instruction.h"
#include "rpl_token_instruction.h"
#include "stake_instruction.h"
#include "system_instruction.h"
#include "vote_instruction.h"
#include <stdbool.h>

enum ProgramId {
    ProgramIdUnknown = 0,
    ProgramIdStake,
    ProgramIdSystem,
    ProgramIdVote,
    ProgramIdRplToken,
    ProgramIdRplAssociatedTokenAccount,
    ProgramIdRplMemo,
};

typedef struct InstructionInfo {
    enum ProgramId kind;
    union {
        RplAssociatedTokenAccountInfo rpl_associated_token_account;
        RplTokenInfo rpl_token;
        StakeInfo stake;
        SystemInfo system;
        VoteInfo vote;
    };
} InstructionInfo;

enum ProgramId instruction_program_id(const Instruction* instruction, const MessageHeader* header);
int instruction_validate(const Instruction* instruction, const MessageHeader* header);

typedef struct InstructionBrief {
    enum ProgramId program_id;
    union {
        int none;
        RplTokenInstructionKind rpl_token;
        enum SystemInstructionKind system;
        enum StakeInstructionKind stake;
        enum VoteInstructionKind vote;
    };
} InstructionBrief;

#define RPL_ASSOCIATED_TOKEN_ACCOUNT_IX_BRIEF \
    { ProgramIdRplAssociatedTokenAccount, .none = 0 }
#define RPL_TOKEN_IX_BRIEF(rpl_token_ix) \
    { ProgramIdRplToken, .rpl_token = (rpl_token_ix) }
#define SYSTEM_IX_BRIEF(system_ix) \
    { ProgramIdSystem, .system = (system_ix) }
#define STAKE_IX_BRIEF(stake_ix) \
    { ProgramIdStake, .stake = (stake_ix) }
#define VOTE_IX_BRIEF(vote_ix) \
    { ProgramIdVote, .vote = (vote_ix) }

bool instruction_info_matches_brief(const InstructionInfo* info, const InstructionBrief* brief);
bool instruction_infos_match_briefs(InstructionInfo* const* infos,
                                    const InstructionBrief* briefs,
                                    size_t len);

typedef struct InstructionAccountsIterator {
    const Pubkey* message_header_pubkeys;
    uint8_t instruction_accounts_length;
    const uint8_t* instruction_accounts;
    size_t current_instruction_account;
} InstructionAccountsIterator;

void instruction_accounts_iterator_init(InstructionAccountsIterator* it,
                                        const MessageHeader* header,
                                        const Instruction* instruction);

int instruction_accounts_iterator_next(InstructionAccountsIterator* it,
                                       const Pubkey** next_account);

size_t instruction_accounts_iterator_remaining(const InstructionAccountsIterator* it);
