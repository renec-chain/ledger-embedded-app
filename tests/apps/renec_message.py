from enum import IntEnum
from typing import List

from base58 import b58decode

from .renec_account import AccountMeta
from .renec_instruction import CompiledInstruction, Instruction
from .renec_utils import FAKE_RECENT_BLOCKHASH

PACKAGE_DATA_SIZE = 1280 - 40 - 8
U16_MAX = 2^16 - 1

SIGNING_DOMAIN = b"\xffrenec offchain"

BASE_HEADER_LEN = len(SIGNING_DOMAIN) + 1

# Header length = Message Format (1) + Message Length (2)
MESSAGE_HEADER_LEN = 3
# Max length of the OffchainMessage
MAX_LEN = U16_MAX - BASE_HEADER_LEN - MESSAGE_HEADER_LEN
# Max length of the OffchainMessage supported by Ledger
MAX_LEN_LEDGER = PACKAGE_DATA_SIZE - BASE_HEADER_LEN - MESSAGE_HEADER_LEN


class MessageHeader:
    def __init__(self,
                 num_required_signatures: int,
                 num_readonly_signed_accounts: int,
                 num_readonly_unsigned_accounts: int):
        self.num_required_signatures = num_required_signatures
        self.num_readonly_signed_accounts = num_readonly_signed_accounts
        self.num_readonly_unsigned_accounts = num_readonly_unsigned_accounts

    def serialize(self) -> bytes:
        return self.num_required_signatures.to_bytes(1, byteorder="little") + \
               self.num_readonly_signed_accounts.to_bytes(1, byteorder="little") + \
               self.num_readonly_unsigned_accounts.to_bytes(1, byteorder="little")
    

class Message:
    header: MessageHeader
    account_keys: List[AccountMeta]
    recent_blockhash: bytes
    compiled_instructions: List[CompiledInstruction]

    def __init__(self, instructions: List[Instruction]):
        # TODO Add support for multiple transfers and other instructions
        self.header = MessageHeader(2, 0, 1)
        self.account_keys = [instructions[0].from_pubkey,
                             instructions[0].to_pubkey,
                             instructions[0].program_id]
        self.recent_blockhash = b58decode(FAKE_RECENT_BLOCKHASH)
        self.compiled_instructions = [CompiledInstruction(2, 
                                                          [0, 1], 
                                                          instructions[0].data)]
        
    def serialize(self) -> bytes:
        serialized = self.header.serialize()
        serialized += len(self.account_keys).to_bytes(1, byteorder="little")
        for account_key in self.account_keys:
            serialized += account_key
        serialized += self.recent_blockhash
        serialized += len(self.compiled_instructions).to_bytes(1, byteorder="little")
        serialized += self.compiled_instructions[0].serialize()

        return serialized


class MessageFormat(IntEnum):
    RestrictedAscii = 0x00
    LimitedUtf8 = 0x01
    ExtendedUtf8 = 0x02


class OffchainMessage:
    format: MessageFormat
    version: int
    message: bytes

    def __init__(self, message: bytes):
        if len(message) <= MAX_LEN_LEDGER:
            if message.isascii():
                self.format = MessageFormat.RestrictedAscii
            else:
                self.format = MessageFormat.LimitedUtf8
        elif len(message) <= MAX_LEN:
            self.format = MessageFormat.ExtendedUtf8
        else:
            raise ValueError()
        # TODO This is always version 0
        self.version = 0
        self.message = message

    def serialize(self) -> bytes:
        data = b""
        # signing domain
        data += SIGNING_DOMAIN
        # version
        data += self.version.to_bytes(1, byteorder="little")
        # format
        data += self.format.to_bytes(1, byteorder="little")
        # message length
        data += len(self.message).to_bytes(2, byteorder="little")
        # message
        data += self.message

        return data
