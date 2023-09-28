from ragger.backend import BackendInterface, RaisePolicy
from ragger.navigator import Navigator, NavInsID

from .apps.renec import ErrorType, RenecClient
from .apps.renec_utils import RENEC_PACKED_DERIVATION_PATH, ROOT_SCREENSHOT_PATH


class TestGetPublicKey:

    def test_renec_get_public_key_ok(self,
                                     backend: BackendInterface,
                                     navigator: Navigator,
                                     test_name: str):
        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)
        
        with renec.send_public_key_with_confirm(RENEC_PACKED_DERIVATION_PATH):
            if backend.firmware.device.startswith("nano"):
                navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                          [NavInsID.BOTH_CLICK],
                                                          "Approve",
                                                          ROOT_SCREENSHOT_PATH,
                                                          test_name)
            else:
                instructions = [
                    NavInsID.USE_CASE_REVIEW_TAP,
                    NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM,
                    NavInsID.USE_CASE_STATUS_DISMISS
                ]
                navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                               test_name,
                                               instructions)

        assert renec.get_async_response().data == from_public_key

    def test_renec_get_public_key_refused(self,
                                          backend: BackendInterface, 
                                          navigator: Navigator, 
                                          test_name: str):
        renec = RenecClient(backend)
        with renec.send_public_key_with_confirm(RENEC_PACKED_DERIVATION_PATH):
            backend.raise_policy = RaisePolicy.RAISE_NOTHING
            if backend.firmware.device.startswith("nano"):
                navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                          [NavInsID.BOTH_CLICK],
                                                          "Reject",
                                                          ROOT_SCREENSHOT_PATH,
                                                          test_name)
            else:
                instructions = [
                    NavInsID.USE_CASE_REVIEW_TAP,
                    NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL,
                    NavInsID.USE_CASE_STATUS_DISMISS
                ]
                navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                               test_name,
                                               instructions)

        assert renec.get_async_response().status == ErrorType.USER_CANCEL
