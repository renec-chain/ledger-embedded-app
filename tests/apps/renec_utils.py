from pathlib import Path

from base58 import b58decode
from nacl.signing import VerifyKey
from ragger.bip import pack_derivation_path

ROOT_SCREENSHOT_PATH = Path(__file__).resolve().parent.parent

PROGRAM_ID_SYSTEM = "11111111111111111111111111111111"

# Fake blockhash so this example doesn't need a network connection. 
# It should be queried from the cluster in normal use.
FAKE_RECENT_BLOCKHASH = "11111111111111111111111111111111"


def renec_to_lamports(renec_amount: int) -> int:
    return round(renec_amount * 10 ** 9)


def verify_signature(from_public_key: bytes, message: bytes, signature: bytes):
    assert len(signature) == 64, "signature size incorrect"
    verify_key = VerifyKey(from_public_key)
    verify_key.verify(message, signature)

AMOUNT          = renec_to_lamports(2.078)

# "Foreign" Solana public key (actually the device
#  public key derived on m/44'/501'/11111')
FOREIGN_ADDRESS = b"AxmUF3qkdz1zs151Q5WttVMkFpFGQPwghZs4d1mwY55d"
FOREIGN_PUBLIC_KEY = b58decode(FOREIGN_ADDRESS)

RENEC_PACKED_DERIVATION_PATH = pack_derivation_path("m/44'/501'/12345'")
