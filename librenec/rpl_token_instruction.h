#pragma once

#include "renec/print_config.h"
#include "renec/transaction_summary.h"
#include "rpl/token.h"

#define RplTokenBody(b) Token_TokenInstruction_Token_##b##_Body
#define RplTokenKind(b) Token_TokenInstruction_##b

#define RplTokenInstructionKind Token_TokenInstruction_Tag

typedef struct RplTokenMultisigners {
    const Pubkey* first;
    uint8_t count;
} RplTokenMultisigners;

typedef enum {
    RplTokenSignKindSingle,
    RplTokenSignKindMulti,
} RplTokenSignKind;

typedef struct {
    RplTokenSignKind kind;
    union {
        struct {
            const Pubkey* signer;
        } single;
        struct {
            const Pubkey* account;
            RplTokenMultisigners signers;
        } multi;
    };
} RplTokenSign;

extern const Pubkey rpl_token_program_id;

typedef struct RplTokenInitializeMintInfo {
    const Pubkey* mint_account;
    const Pubkey* mint_authority;
    const Pubkey* freeze_authority;
    uint8_t decimals;
} RplTokenInitializeMintInfo;

typedef struct RplTokenInitializeAccountInfo {
    const Pubkey* token_account;
    const Pubkey* mint_account;
    const Pubkey* owner;
} RplTokenInitializeAccountInfo;

typedef struct RplTokenInitializeMultisigInfo {
    const Pubkey* multisig_account;
    RplTokenMultisigners signers;
    RplTokenBody(InitializeMultisig) body;
} RplTokenInitializeMultisigInfo;

typedef struct RplTokenTransferInfo {
    const Pubkey* src_account;
    const Pubkey* dest_account;
    const Pubkey* mint_account;
    RplTokenSign sign;
    RplTokenBody(TransferChecked) body;
} RplTokenTransferInfo;

typedef struct RplTokenApproveInfo {
    const Pubkey* token_account;
    const Pubkey* delegate;
    const Pubkey* mint_account;
    RplTokenSign sign;
    RplTokenBody(ApproveChecked) body;
} RplTokenApproveInfo;

typedef struct RplTokenRevokeInfo {
    const Pubkey* token_account;
    RplTokenSign sign;
} RplTokenRevokeInfo;

typedef struct RplTokenSetAuthorityInfo {
    const Pubkey* account;
    const Pubkey* new_authority;
    Token_AuthorityType authority_type;
    RplTokenSign sign;
} RplTokenSetAuthorityInfo;

typedef struct RplTokenMintToInfo {
    const Pubkey* mint_account;
    const Pubkey* token_account;
    RplTokenSign sign;
    RplTokenBody(MintToChecked) body;
} RplTokenMintToInfo;

typedef struct RplTokenBurnInfo {
    const Pubkey* token_account;
    const Pubkey* mint_account;
    RplTokenSign sign;
    RplTokenBody(BurnChecked) body;
} RplTokenBurnInfo;

typedef struct RplTokenCloseAccountInfo {
    const Pubkey* token_account;
    const Pubkey* dest_account;
    RplTokenSign sign;
} RplTokenCloseAccountInfo;

typedef struct RplTokenFreezeAccountInfo {
    const Pubkey* token_account;
    const Pubkey* mint_account;
    RplTokenSign sign;
} RplTokenFreezeAccountInfo;

typedef struct RplTokenThawAccountInfo {
    const Pubkey* token_account;
    const Pubkey* mint_account;
    RplTokenSign sign;
} RplTokenThawAccountInfo;

typedef struct RplTokenSyncNativeInfo {
    const Pubkey* token_account;
} RplTokenSyncNativeInfo;

typedef struct RplTokenInfo {
    RplTokenInstructionKind kind;
    union {
        RplTokenInitializeMintInfo initialize_mint;
        RplTokenInitializeAccountInfo initialize_account;
        RplTokenInitializeMultisigInfo initialize_multisig;
        RplTokenTransferInfo transfer;
        RplTokenApproveInfo approve;
        RplTokenRevokeInfo revoke;
        RplTokenSetAuthorityInfo set_owner;
        RplTokenMintToInfo mint_to;
        RplTokenBurnInfo burn;
        RplTokenCloseAccountInfo close_account;
        RplTokenFreezeAccountInfo freeze_account;
        RplTokenThawAccountInfo thaw_account;
        RplTokenSyncNativeInfo sync_native;
    };
} RplTokenInfo;

int parse_rpl_token_instructions(const Instruction* instruction,
                                 const MessageHeader* header,
                                 RplTokenInfo* info);
int print_rpl_token_info(const RplTokenInfo* info, const PrintConfig* print_config);
void summary_item_set_multisig_m_of_n(SummaryItem* item, uint8_t m, uint8_t n);

#define RplTokenOptionPubkeyKind      Token_COption_Pubkey_Tag
#define RplTokenToOptionPubkeyKind(k) Token_COption_Pubkey_##k##_Pubkey
#define RplTokenOptionPubkeyBody      Token_COption_Pubkey_Token_Some_Body_Pubkey
#define RplTokenOptionPubkey          Token_COption_Pubkey
const Pubkey* rpl_token_option_pubkey_get(const RplTokenOptionPubkey* option_pubkey);

int print_rpl_token_transfer_info(const RplTokenTransferInfo* info,
                                  const PrintConfig* print_config,
                                  bool primary);
