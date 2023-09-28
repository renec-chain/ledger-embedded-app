from contextlib import contextmanager
from enum import IntEnum

from ragger.backend.interface import RAPDU, BackendInterface

CLA: int = 0xE0

PUBLIC_KEY_LENGTH = 32

MAX_CHUNK_SIZE = 255

STATUS_OK = 0x9000


class P1(IntEnum):
    P1_NON_CONFIRM = 0x00
    P1_CONFIRM = 0x01

class P2(IntEnum):
    P2_NONE = 0x00
    P2_EXTEND = 0x01
    P2_MORE = 0x02


class INS(IntEnum):
    INS_GET_APP_CONFIGURATION = 0x03
    INS_GET_PUBKEY = 0x04
    INS_SIGN_MESSAGE = 0x05
    INS_SIGN_OFFCHAIN_MESSAGE = 0x06


class ErrorType(IntEnum):
    NO_APP_RESPONSE = 0x6700
    SDK_EXCEPTION = 0x6801
    SDK_INVALID_PARAMETER = 0x6802
    SDK_EXCEPTION_OVERFLOW = 0x6803
    SDK_EXCEPTION_SECURITY = 0x6804
    SDK_INVALID_CRC = 0x6805
    SDK_INVALID_CHECKSUM = 0x6806
    SDK_INVALID_COUNTER = 0x6807
    SDK_NOT_SUPPORTED = 0x6808
    SDK_INVALID_STATE = 0x6809
    SDK_TIMEOUT = 0x6810
    SDK_EXCEPTION_PIC = 0x6811
    SDK_EXCEPTION_APP_EXIT = 0x6812
    SDK_EXCEPTION_IO_OVERFLOW = 0x6813
    SDK_EXCEPTION_IO_HEADER = 0x6814
    SDK_EXCEPTION_IO_STATE = 0x6815
    SDK_EXCEPTION_IO_RESET = 0x6816
    SDK_EXCEPTION_CX_PORT = 0x6817
    SDK_EXCEPTION_SYSTEM = 0x6818
    SDK_NOT_ENOUGH_SPACE = 0x6819
    NO_APDU_RECEIVED = 0x6982
    USER_CANCEL = 0x6985
    RENEC_INVALID_MESSAGE = 0x6a80
    RENEC_SUMMARY_FINALIZE_FAILED = 0x6f00
    RENEC_SUMMARY_UPDATE_FAILED = 0x6f01
    UNIMPLEMENTED_INSTRUCTION = 0x6d00
    INVALID_CLA = 0x6e00


class RenecClient:
    client: BackendInterface

    def __init__(self, client: BackendInterface) -> None:
        self._client = client

    def get_public_key(self, derivation_path: bytes) -> bytes:
        public_key: RAPDU = self._client.exchange(CLA, INS.INS_GET_PUBKEY,
                                                  P1.P1_NON_CONFIRM, P2.P2_NONE,
                                                  derivation_path)
        
        assert len(public_key.data) == PUBLIC_KEY_LENGTH, \
            "'from' public key size incorrect"
        return public_key.data
    
    @contextmanager
    def send_public_key_with_confirm(self, derivation_path: bytes) -> bytes:
        with self._client.exchange_async(CLA, INS.INS_GET_PUBKEY,
                                         P1.P1_CONFIRM, P2.P2_NONE,
                                         derivation_path):
            yield
    
    def get_async_response(self) -> RAPDU:
        return self._client.last_async_response
