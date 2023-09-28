import logging

from ragger.backend import BackendInterface
from ragger.navigator import Navigator

from .apps.navigator_utils import enable_blind_signing, navigation_helper_confirm
from .apps.renec import RenecClient
from .apps.renec_message import OffchainMessage
from .apps.renec_utils import (
    RENEC_PACKED_DERIVATION_PATH,
    verify_signature,
)

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)


# class TestMessageSigning:

#     def test_renec_simple_transfer_ok_1(self,
#                                         backend: BackendInterface,
#                                         navigator: Navigator, 
#                                         test_name: str):
#         renec = RenecClient(backend)
#         from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

#         instruction = SystemInstructionTransfer(from_public_key, 
#                                                 FOREIGN_PUBLIC_KEY,
#                                                 AMOUNT)
#         message = Message([instruction]).serialize()

#         with renec.send_async_sign_message(RENEC_PACKED_DERIVATION_PATH, message):
#             navigation_helper_confirm(navigator, backend.firmware.name, test_name)

#         signature = renec.get_async_response().data
#         logger.info(f"Signature: {signature}")
#         verify_signature(from_public_key, message, signature)


class TestOffchainMessageSigning:

    # def test_ledger_sign_offchain_message_ascii_ok(self, 
    #                                                backend: BackendInterface, 
    #                                                navigator: Navigator, 
    #                                                test_name: str):
    #     renec = RenecClient(backend)
    #     from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

    #     offchain_message = OffchainMessage(b"Test message")
    #     message = offchain_message.serialize()

    #     with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, 
    #                                                 message):
    #         navigation_helper_confirm(navigator, backend.firmware.name, test_name)

    #     signature = renec.get_async_response().data
    #     verify_signature(from_public_key, message, signature)

    def test_ledger_sign_offchain_message_utf8_ok(self,
                                                  backend: BackendInterface,
                                                  navigator: Navigator,
                                                  test_name: str):
        enable_blind_signing(navigator, backend.firmware.name, test_name + "_1")

        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)
        
        offchain_message = OffchainMessage(bytes("Тестовое сообщение", "utf-8"))
        message = offchain_message.serialize()

        renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, 
                                                    message)
        navigation_helper_confirm(navigator, 
                                  backend.firmware.device, 
                                  test_name + "_2")
        
        signature = renec.get_async_response().data
        verify_signature(from_public_key, message, signature)
