from pathlib import Path
from ragger.bip import pack_derivation_path

ROOT_SCREENSHOT_PATH = Path(__file__).resolve().parent.parent

RENEC_PACKED_DERIVATION_PATH = pack_derivation_path("m/44'/501'/12345'")
