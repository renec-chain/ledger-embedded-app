#include "common_byte_strings.h"
#include "instruction.h"
#include "renec/parser.h"
#include "renec/transaction_summary.h"
#include "rpl_associated_token_account_instruction.h"
#include "util.h"

const Pubkey rpl_associated_token_account_program_id = {{PROGRAM_ID_RPL_ASSOCIATED_TOKEN_ACCOUNT}};

static int parse_create_rpl_associated_token_account_instruction(
    const Instruction* instruction,
    const MessageHeader* header,
    RplAssociatedTokenAccountCreateInfo* info) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->funder));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->address));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->owner));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint));
    // Skip system program_id
    BAIL_IF(instruction_accounts_iterator_next(&it, NULL));
    // Skip rpl token program_id
    BAIL_IF(instruction_accounts_iterator_next(&it, NULL));
    // Skip (optional) rent sysvar
    instruction_accounts_iterator_next(&it, NULL);

    return 0;
}

int parse_rpl_associated_token_account_instructions(const Instruction* instruction,
                                                    const MessageHeader* header,
                                                    RplAssociatedTokenAccountInfo* info) {
    return parse_create_rpl_associated_token_account_instruction(instruction,
                                                                 header,
                                                                 &info->create);
}

int print_rpl_associated_token_account_create_info(const RplAssociatedTokenAccountCreateInfo* info,
                                                   const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create token acct", info->address);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From mint", info->mint);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Owned by", info->owner);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Funded by", info->funder);

    return 0;
}

int print_rpl_associated_token_account_info(const RplAssociatedTokenAccountInfo* info,
                                            const PrintConfig* print_config) {
    return print_rpl_associated_token_account_create_info(&info->create, print_config);
}
