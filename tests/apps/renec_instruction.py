from enum import IntEnum
from typing import List

from base58 import b58decode

from .renec_account import AccountMeta
from .renec_utils import PROGRAM_ID_SYSTEM


class Instruction:
    program_id: bytes
    accounts: List[AccountMeta]
    data: bytes
    from_pubkey: bytes
    to_pubkey: bytes


class SystemInstruction(IntEnum):
    CreateAccount           = 0x00
    Assign                  = 0x01
    Transfer                = 0x02
    CreateAccountWithSeed   = 0x03
    AdvanceNonceAccount     = 0x04
    WithdrawNonceAccount    = 0x05
    InitializeNonceAccount  = 0x06
    AuthorizeNonceAccount   = 0x07
    Allocate                = 0x08
    AllocateWithSeed        = 0x09
    AssignWithSeed          = 0x10
    TransferWithSeed        = 0x11
    UpgradeNonceAccount     = 0x12


class SystemInstructionTransfer(Instruction):
    def __init__(self, from_pubkey: bytes, to_pubkey: bytes, amount: int):
        self.from_pubkey = from_pubkey
        self.to_pubkey = to_pubkey
        self.program_id = b58decode(PROGRAM_ID_SYSTEM)
        self.accounts = [AccountMeta(from_pubkey, True, True), 
                         AccountMeta(to_pubkey, False, True)]
        self.data = (SystemInstruction.Transfer).to_bytes(4, byteorder="little") + \
            (amount).to_bytes(8, byteorder="little")
        

# TODO Add support for multiple transfers and other instructions
class CompiledInstruction:
    program_id_index: int
    accounts: List[int]
    data: bytes

    def __init__(self, program_id_index: int, accounts: List[int], data: bytes):
        self.program_id_index = program_id_index
        self.accounts = accounts
        self.data = data

    def serialize(self) -> bytes:
        serialized = self.program_id_index.to_bytes(1, byteorder="little")
        serialized += len(self.accounts).to_bytes(1, byteorder="little")
        for account in self.accounts:
            serialized += (account).to_bytes(1, byteorder="little")
        serialized += len(self.data).to_bytes(1, byteorder="little")
        serialized += self.data

        return serialized
