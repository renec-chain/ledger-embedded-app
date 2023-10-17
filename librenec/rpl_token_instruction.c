#include "common_byte_strings.h"
#include "instruction.h"
#include "renec/parser.h"
#include "renec/transaction_summary.h"
#include "rpl_token_instruction.h"
#include "token_info.h"
#include "util.h"

const Pubkey rpl_token_program_id = {{PROGRAM_ID_RPL_TOKEN}};

static int parse_rpl_token_instruction_kind(Parser* parser, RplTokenInstructionKind* kind) {
    uint8_t maybe_kind;
    BAIL_IF(parse_u8(parser, &maybe_kind));
    switch (maybe_kind) {
        case RplTokenKind(InitializeMint):
        case RplTokenKind(InitializeAccount):
        case RplTokenKind(InitializeAccount2):
        case RplTokenKind(InitializeMultisig):
        case RplTokenKind(TransferChecked):
        case RplTokenKind(ApproveChecked):
        case RplTokenKind(Revoke):
        case RplTokenKind(SetAuthority):
        case RplTokenKind(MintToChecked):
        case RplTokenKind(BurnChecked):
        case RplTokenKind(CloseAccount):
        case RplTokenKind(FreezeAccount):
        case RplTokenKind(ThawAccount):
        case RplTokenKind(SyncNative):
            *kind = (RplTokenInstructionKind) maybe_kind;
            return 0;
        // Deprecated instructions
        case RplTokenKind(Transfer):
        case RplTokenKind(Approve):
        case RplTokenKind(MintTo):
        case RplTokenKind(Burn):
            break;
    }
    return 1;
}

static int parse_initialize_mint_rpl_token_instruction(Parser* parser,
                                                       const Instruction* instruction,
                                                       const MessageHeader* header,
                                                       RplTokenInitializeMintInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u8(parser, &info->decimals));
    BAIL_IF(parse_pubkey(parser, &info->mint_authority));
    enum Option freeze_authority;
    BAIL_IF(parse_option(parser, &freeze_authority));
    if (freeze_authority == OptionSome) {
        BAIL_IF(parse_pubkey(parser, &info->freeze_authority));
    } else {
        info->freeze_authority = NULL;
    }

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    // Skip rent sysvar
    BAIL_IF(instruction_accounts_iterator_next(&it, NULL));

    return 0;
}

static int parse_initialize_account_rpl_token_instruction(Parser* parser,
                                                          const Instruction* instruction,
                                                          const MessageHeader* header,
                                                          RplTokenInitializeAccountInfo* info,
                                                          bool expect_owner_in_accounts) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    if (expect_owner_in_accounts) {
        BAIL_IF(instruction->accounts_length != 4);
    }

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    if (expect_owner_in_accounts) {
        BAIL_IF(instruction_accounts_iterator_next(&it, &info->owner));
    }
    // Skip rent sysvar
    BAIL_IF(instruction_accounts_iterator_next(&it, NULL));

    if (!expect_owner_in_accounts) {
        BAIL_IF(parse_pubkey(parser, &info->owner));
    }

    return 0;
}

static int parse_rpl_token_multisigners(InstructionAccountsIterator* it,
                                        RplTokenMultisigners* signers) {
    size_t n = instruction_accounts_iterator_remaining(it);
    BAIL_IF(n > Token_MAX_SIGNERS);
    BAIL_IF(instruction_accounts_iterator_next(it, &signers->first));
    signers->count = n;

    return 0;
}

static int parse_initialize_multisig_rpl_token_instruction(Parser* parser,
                                                           const Instruction* instruction,
                                                           const MessageHeader* header,
                                                           RplTokenInitializeMultisigInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u8(parser, &info->body.m));
    BAIL_IF(info->body.m > Token_MAX_SIGNERS);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->multisig_account));
    // Skip rent sysvar
    BAIL_IF(instruction_accounts_iterator_next(&it, NULL));
    BAIL_IF(parse_rpl_token_multisigners(&it, &info->signers));

    return 0;
}

static int parse_rpl_token_sign(InstructionAccountsIterator* it, RplTokenSign* sign) {
    size_t n = instruction_accounts_iterator_remaining(it);
    BAIL_IF(n == 0);

    if (n == 1) {
        sign->kind = RplTokenSignKindSingle;
        BAIL_IF(instruction_accounts_iterator_next(it, &sign->single.signer));
    } else {
        sign->kind = RplTokenSignKindMulti;
        BAIL_IF(instruction_accounts_iterator_next(it, &sign->multi.account));
        BAIL_IF(parse_rpl_token_multisigners(it, &sign->multi.signers));
    }
    return 0;
}

static int parse_transfer_rpl_token_instruction(Parser* parser,
                                                const Instruction* instruction,
                                                const MessageHeader* header,
                                                RplTokenTransferInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));
    BAIL_IF(parse_u8(parser, &info->body.decimals));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->src_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->dest_account));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_approve_rpl_token_instruction(Parser* parser,
                                               const Instruction* instruction,
                                               const MessageHeader* header,
                                               RplTokenApproveInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));
    BAIL_IF(parse_u8(parser, &info->body.decimals));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->delegate));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_revoke_rpl_token_instruction(const Instruction* instruction,
                                              const MessageHeader* header,
                                              RplTokenRevokeInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_token_authority_type(Parser* parser, Token_AuthorityType* auth_type) {
    uint8_t maybe_type;
    BAIL_IF(parse_u8(parser, &maybe_type));
    switch (maybe_type) {
        case Token_AuthorityType_MintTokens:
        case Token_AuthorityType_FreezeAccount:
        case Token_AuthorityType_AccountOwner:
        case Token_AuthorityType_CloseAccount:
            *auth_type = (Token_AuthorityType) maybe_type;
            return 0;
    }
    return 1;
}

static const char* stringify_token_authority_type(Token_AuthorityType auth_type) {
    switch (auth_type) {
        case Token_AuthorityType_MintTokens:
            return "Mint tokens";
        case Token_AuthorityType_FreezeAccount:
            return "Freeze account";
        case Token_AuthorityType_AccountOwner:
            return "Owner";
        case Token_AuthorityType_CloseAccount:
            return "Close acct";
    }
    return NULL;
}

static int parse_set_authority_rpl_token_instruction(Parser* parser,
                                                     const Instruction* instruction,
                                                     const MessageHeader* header,
                                                     RplTokenSetAuthorityInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->account));

    BAIL_IF(parse_token_authority_type(parser, &info->authority_type));

    enum Option new_authority;
    BAIL_IF(parse_option(parser, &new_authority));
    if (new_authority == OptionSome) {
        BAIL_IF(parse_pubkey(parser, &info->new_authority));
    } else {
        info->new_authority = NULL;
    }

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_mint_to_rpl_token_instruction(Parser* parser,
                                               const Instruction* instruction,
                                               const MessageHeader* header,
                                               RplTokenMintToInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));
    BAIL_IF(parse_u8(parser, &info->body.decimals));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_burn_rpl_token_instruction(Parser* parser,
                                            const Instruction* instruction,
                                            const MessageHeader* header,
                                            RplTokenBurnInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));
    BAIL_IF(parse_u8(parser, &info->body.decimals));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_close_account_rpl_token_instruction(const Instruction* instruction,
                                                     const MessageHeader* header,
                                                     RplTokenCloseAccountInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->dest_account));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_freeze_account_rpl_token_instruction(const Instruction* instruction,
                                                      const MessageHeader* header,
                                                      RplTokenFreezeAccountInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_thaw_account_rpl_token_instruction(const Instruction* instruction,
                                                    const MessageHeader* header,
                                                    RplTokenThawAccountInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));

    BAIL_IF(parse_rpl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_sync_native_rpl_token_instruction(const Instruction* instruction,
                                                   const MessageHeader* header,
                                                   RplTokenSyncNativeInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));

    return 0;
}

int parse_rpl_token_instructions(const Instruction* instruction,
                                 const MessageHeader* header,
                                 RplTokenInfo* info) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_rpl_token_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case RplTokenKind(InitializeMint):
            return parse_initialize_mint_rpl_token_instruction(&parser,
                                                               instruction,
                                                               header,
                                                               &info->initialize_mint);
        case RplTokenKind(InitializeAccount):
            return parse_initialize_account_rpl_token_instruction(&parser,
                                                                  instruction,
                                                                  header,
                                                                  &info->initialize_account,
                                                                  true);
        case RplTokenKind(InitializeAccount2):
            return parse_initialize_account_rpl_token_instruction(&parser,
                                                                  instruction,
                                                                  header,
                                                                  &info->initialize_account,
                                                                  false);
        case RplTokenKind(InitializeMultisig):
            return parse_initialize_multisig_rpl_token_instruction(&parser,
                                                                   instruction,
                                                                   header,
                                                                   &info->initialize_multisig);
        case RplTokenKind(Revoke):
            return parse_revoke_rpl_token_instruction(instruction, header, &info->revoke);
        case RplTokenKind(SetAuthority):
            return parse_set_authority_rpl_token_instruction(&parser,
                                                             instruction,
                                                             header,
                                                             &info->set_owner);
        case RplTokenKind(CloseAccount):
            return parse_close_account_rpl_token_instruction(instruction,
                                                             header,
                                                             &info->close_account);
        case RplTokenKind(FreezeAccount):
            return parse_freeze_account_rpl_token_instruction(instruction,
                                                              header,
                                                              &info->freeze_account);
        case RplTokenKind(ThawAccount):
            return parse_thaw_account_rpl_token_instruction(instruction,
                                                            header,
                                                            &info->thaw_account);
        case RplTokenKind(TransferChecked):
            return parse_transfer_rpl_token_instruction(&parser,
                                                        instruction,
                                                        header,
                                                        &info->transfer);
        case RplTokenKind(ApproveChecked):
            return parse_approve_rpl_token_instruction(&parser,
                                                       instruction,
                                                       header,
                                                       &info->approve);
        case RplTokenKind(MintToChecked):
            return parse_mint_to_rpl_token_instruction(&parser,
                                                       instruction,
                                                       header,
                                                       &info->mint_to);
        case RplTokenKind(BurnChecked):
            return parse_burn_rpl_token_instruction(&parser, instruction, header, &info->burn);
        case RplTokenKind(SyncNative):
            return parse_sync_native_rpl_token_instruction(instruction, header, &info->sync_native);
        // Deprecated instructions
        case RplTokenKind(Transfer):
        case RplTokenKind(Approve):
        case RplTokenKind(MintTo):
        case RplTokenKind(Burn):
            break;
    }
    return 1;
}

static int print_rpl_token_sign(const RplTokenSign* sign, const PrintConfig* print_config) {
    SummaryItem* item;

    item = transaction_summary_general_item();
    if (sign->kind == RplTokenSignKindSingle) {
        if (print_config_show_authority(print_config, sign->single.signer)) {
            summary_item_set_pubkey(item, "Owner", sign->single.signer);
        }
    } else {
        summary_item_set_pubkey(item, "Owner", sign->multi.account);
        item = transaction_summary_general_item();
        summary_item_set_u64(item, "Signers", sign->multi.signers.count);
    }

    return 0;
}

static int print_rpl_token_initialize_mint_info(const char* primary_title,
                                                const RplTokenInitializeMintInfo* info,
                                                const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->mint_account);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Mint authority", info->mint_authority);

    item = transaction_summary_general_item();
    summary_item_set_u64(item, "Decimals", info->decimals);

    if (info->freeze_authority != NULL) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "Freeze authority", info->freeze_authority);
    }

    return 0;
}

static int print_rpl_token_initialize_account_info(const char* primary_title,
                                                   const RplTokenInitializeAccountInfo* info,
                                                   const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->token_account);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Owner", info->owner);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Mint", info->mint_account);

    return 0;
}

static int print_rpl_token_initialize_multisig_info(const char* primary_title,
                                                    const RplTokenInitializeMultisigInfo* info,
                                                    const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->multisig_account);
    }

    item = transaction_summary_general_item();
    summary_item_set_multisig_m_of_n(item, info->body.m, info->signers.count);

    return 0;
}

int print_rpl_token_transfer_info(const RplTokenTransferInfo* info,
                                  const PrintConfig* print_config,
                                  bool primary) {
    SummaryItem* item;

    if (primary) {
        item = transaction_summary_primary_item();
    } else {
        item = transaction_summary_general_item();
    }

    const char* symbol = get_token_symbol(info->mint_account);
    summary_item_set_token_amount(item,
                                  "Transfer tokens",
                                  info->body.amount,
                                  symbol,
                                  info->body.decimals);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From", info->src_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "To", info->dest_account);

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_approve_info(const RplTokenApproveInfo* info,
                                        const PrintConfig* print_config) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Approve delegate", info->delegate);

    item = transaction_summary_general_item();
    const char* symbol = get_token_symbol(info->mint_account);
    summary_item_set_token_amount(item,
                                  "Allowance",
                                  info->body.amount,
                                  symbol,
                                  info->body.decimals);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From", info->token_account);

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_revoke_info(const RplTokenRevokeInfo* info,
                                       const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Revoke delegate", info->token_account);

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_set_authority_info(const RplTokenSetAuthorityInfo* info,
                                              const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;
    bool clear_authority = info->new_authority == NULL;
    const char* primary_title = "Set authority";
    if (clear_authority) {
        primary_title = "Clear authority";
    }

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, primary_title, info->account);

    const char* authority_type = stringify_token_authority_type(info->authority_type);
    BAIL_IF(authority_type == NULL);
    item = transaction_summary_general_item();
    summary_item_set_string(item, "Type", authority_type);

    if (!clear_authority) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "Authority", info->new_authority);
    }

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_mint_to_info(const RplTokenMintToInfo* info,
                                        const PrintConfig* print_config) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    const char* symbol = get_token_symbol(info->mint_account);
    summary_item_set_token_amount(item,
                                  "Mint tokens",
                                  info->body.amount,
                                  symbol,
                                  info->body.decimals);

    if (print_config->expert_mode) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "From", info->mint_account);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "To", info->token_account);

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_burn_info(const RplTokenBurnInfo* info,
                                     const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    item = transaction_summary_primary_item();
    const char* symbol = get_token_symbol(info->mint_account);
    summary_item_set_token_amount(item,
                                  "Burn tokens",
                                  info->body.amount,
                                  symbol,
                                  info->body.decimals);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From", info->token_account);

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_close_account_info(const RplTokenCloseAccountInfo* info,
                                              const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Close acct", info->token_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Withdraw to", info->dest_account);

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_freeze_account_info(const RplTokenFreezeAccountInfo* info,
                                               const PrintConfig* print_config) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Freeze acct", info->token_account);

    if (print_config->expert_mode) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "Mint", info->mint_account);
    }

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_thaw_account_info(const RplTokenThawAccountInfo* info,
                                             const PrintConfig* print_config) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Thaw acct", info->token_account);

    if (print_config->expert_mode) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "Mint", info->mint_account);
    }

    print_rpl_token_sign(&info->sign, print_config);

    return 0;
}

static int print_rpl_token_sync_native_info(const RplTokenSyncNativeInfo* info,
                                            const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Sync native acct", info->token_account);

    return 0;
}

int print_rpl_token_info(const RplTokenInfo* info, const PrintConfig* print_config) {
    switch (info->kind) {
        case RplTokenKind(InitializeMint):
            return print_rpl_token_initialize_mint_info("Init mint",
                                                        &info->initialize_mint,
                                                        print_config);
        case RplTokenKind(InitializeAccount):
        case RplTokenKind(InitializeAccount2):
            return print_rpl_token_initialize_account_info("Init acct",
                                                           &info->initialize_account,
                                                           print_config);
        case RplTokenKind(InitializeMultisig):
            return print_rpl_token_initialize_multisig_info("Init multisig",
                                                            &info->initialize_multisig,
                                                            print_config);
        case RplTokenKind(Revoke):
            return print_rpl_token_revoke_info(&info->revoke, print_config);
        case RplTokenKind(SetAuthority):
            return print_rpl_token_set_authority_info(&info->set_owner, print_config);
        case RplTokenKind(CloseAccount):
            return print_rpl_token_close_account_info(&info->close_account, print_config);
        case RplTokenKind(FreezeAccount):
            return print_rpl_token_freeze_account_info(&info->freeze_account, print_config);
        case RplTokenKind(ThawAccount):
            return print_rpl_token_thaw_account_info(&info->thaw_account, print_config);
        case RplTokenKind(TransferChecked):
            return print_rpl_token_transfer_info(&info->transfer, print_config, true);
        case RplTokenKind(ApproveChecked):
            return print_rpl_token_approve_info(&info->approve, print_config);
        case RplTokenKind(MintToChecked):
            return print_rpl_token_mint_to_info(&info->mint_to, print_config);
        case RplTokenKind(BurnChecked):
            return print_rpl_token_burn_info(&info->burn, print_config);
        case RplTokenKind(SyncNative):
            return print_rpl_token_sync_native_info(&info->sync_native, print_config);
        // Deprecated instructions
        case RplTokenKind(Transfer):
        case RplTokenKind(Approve):
        case RplTokenKind(MintTo):
        case RplTokenKind(Burn):
            break;
    }

    return 1;
}

#define M_OF_N_MAX_LEN 9  // "11 of 11" + NUL
static int print_m_of_n_string(uint8_t m, uint8_t n, char* buf, size_t buflen) {
    BAIL_IF(n > Token_MAX_SIGNERS);
    BAIL_IF(m > n);
    BAIL_IF(buflen < M_OF_N_MAX_LEN);

    size_t i = 0;
    if (m > 9) buf[i++] = '1';
    buf[i++] = '0' + (m % 10);
    strncpy(&buf[i], " of ", 5);
    i += 4;
    if (n > 9) buf[i++] = '1';
    buf[i++] = '0' + (n % 10);
    buf[i] = '\0';

    return 0;
}

void summary_item_set_multisig_m_of_n(SummaryItem* item, uint8_t m, uint8_t n) {
    static char m_of_n[M_OF_N_MAX_LEN];

    if (print_m_of_n_string(m, n, m_of_n, sizeof(m_of_n)) == 0) {
        summary_item_set_string(item, "Required signers", m_of_n);
    }
}

const Pubkey* rpl_token_option_pubkey_get(const RplTokenOptionPubkey* option_pubkey) {
    switch (option_pubkey->tag) {
        case RplTokenToOptionPubkeyKind(None):
            break;
        case RplTokenToOptionPubkeyKind(Some):
            return (const Pubkey*) &option_pubkey->some;
    }
    return NULL;
}
