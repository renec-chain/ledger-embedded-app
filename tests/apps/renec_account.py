class AccountMeta:
    pubkey: bytes
    is_signer: bool
    is_writable: bool

    def __init__(self, pubkey: bytes, is_signer: bool, is_writable: bool):
        self.pubkey = pubkey
        self.is_signer = is_signer
        self.is_writable = is_writable
