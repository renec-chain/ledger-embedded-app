from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID, NavIns
from ragger.utils import RAPDU

from .apps.renec import RenecClient, ErrorType
from .apps.renec_cmd_builder import SystemInstructionTransfer, Message, verify_signature, OffchainMessage
from .apps.renec_utils import FOREIGN_PUBLIC_KEY, FOREIGN_PUBLIC_KEY_2, AMOUNT, AMOUNT_2, RENEC_PACKED_DERIVATION_PATH, RENEC_PACKED_DERIVATION_PATH_2, ROOT_SCREENSHOT_PATH
from .apps.renec_utils import enable_blind_signing, enable_short_public_key, enable_expert_mode, navigation_helper_confirm, navigation_helper_reject


class TestGetPublicKey:

    def test_renec_get_public_key_ok(self, backend, navigator, test_name):
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
                navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                               test_name,
                                               [NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM])

        assert renec.get_async_response().data == from_public_key


    def test_renec_get_public_key_refused(self, backend, navigator, test_name):
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
                navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                               test_name,
                                               [NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL])

        assert renec.get_async_response().status == ErrorType.USER_CANCEL


class TestMessageSigning:

    def test_renec_simple_transfer_ok_1(self, backend, navigator, test_name):
        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        # Create instruction
        instruction: SystemInstructionTransfer = SystemInstructionTransfer(from_public_key, FOREIGN_PUBLIC_KEY, AMOUNT)
        message: bytes = Message([instruction]).serialize()

        with renec.send_async_sign_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_confirm(navigator, backend.firmware.device, test_name)

        signature: bytes = renec.get_async_response().data
        verify_signature(from_public_key, message, signature)


    def test_renec_simple_transfer_ok_2(self, backend, navigator, test_name):
        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH_2)

        # Create instruction
        instruction: SystemInstructionTransfer = SystemInstructionTransfer(from_public_key, FOREIGN_PUBLIC_KEY_2, AMOUNT_2)
        message: bytes = Message([instruction]).serialize()

        with renec.send_async_sign_message(RENEC_PACKED_DERIVATION_PATH_2, message):
            navigation_helper_confirm(navigator, backend.firmware.device, test_name)

        signature: bytes = renec.get_async_response().data
        verify_signature(from_public_key, message, signature)


    def test_renec_simple_transfer_refused(self, backend, navigator, test_name):
        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        instruction: SystemInstructionTransfer = SystemInstructionTransfer(from_public_key, FOREIGN_PUBLIC_KEY, AMOUNT)
        message: bytes = Message([instruction]).serialize()

        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        with renec.send_async_sign_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_reject(navigator, backend.firmware.device, test_name)

        rapdu: RAPDU = renec.get_async_response()
        assert rapdu.status == ErrorType.USER_CANCEL


class TestOffchainMessageSigning:

    def test_ledger_sign_offchain_message_ascii_ok(self, backend, navigator, test_name):
        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, b"Test message")
        message: bytes = offchain_message.serialize()

        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_confirm(navigator, backend.firmware.device, test_name)

        signature: bytes = renec.get_async_response().data
        verify_signature(from_public_key, message, signature)


    def test_ledger_sign_offchain_message_ascii_refused(self, backend, navigator, test_name):
        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, b"Test message")
        message: bytes = offchain_message.serialize()

        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_reject(navigator, backend.firmware.device, test_name)

        rapdu: RAPDU = renec.get_async_response()
        assert rapdu.status == ErrorType.USER_CANCEL


    def test_ledger_sign_offchain_message_ascii_expert_ok(self, backend, navigator, test_name):
        enable_expert_mode(navigator, backend.firmware.device, test_name + "_1")

        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, b"Test message")
        message: bytes = offchain_message.serialize()

        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_confirm(navigator, backend.firmware.device, test_name + "_2")

        signature: bytes = renec.get_async_response().data
        verify_signature(from_public_key, message, signature)


    def test_ledger_sign_offchain_message_ascii_expert_refused(self, backend, navigator, test_name):
        enable_expert_mode(navigator, backend.firmware.device, test_name + "_1")

        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, b"Test message")
        message: bytes = offchain_message.serialize()

        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_reject(navigator, backend.firmware.device, test_name + "_2")

        rapdu: RAPDU = renec.get_async_response()
        assert rapdu.status == ErrorType.USER_CANCEL


    def test_ledger_sign_offchain_message_utf8_ok(self, backend, navigator, test_name):
        enable_blind_signing(navigator, backend.firmware.device, test_name + "_1")

        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, bytes("Тестовое сообщение", 'utf-8'))
        message: bytes = offchain_message.serialize()

        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_confirm(navigator, backend.firmware.device, test_name + "_2")

        signature: bytes = renec.get_async_response().data
        verify_signature(from_public_key, message, signature)


    def test_ledger_sign_offchain_message_utf8_refused(self, backend, navigator, test_name):
        enable_blind_signing(navigator, backend.firmware.device, test_name + "_1")

        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, bytes("Тестовое сообщение", 'utf-8'))
        message: bytes = offchain_message.serialize()

        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_reject(navigator, backend.firmware.device, test_name + "_2")

        rapdu: RAPDU = renec.get_async_response()
        assert rapdu.status == ErrorType.USER_CANCEL


    def test_ledger_sign_offchain_message_utf8_expert_ok(self, backend, navigator, test_name):
        enable_blind_signing(navigator, backend.firmware.device, test_name + "_1")
        enable_expert_mode(navigator, backend.firmware.device, test_name + "_2")

        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, bytes("Тестовое сообщение", 'utf-8'))
        message: bytes = offchain_message.serialize()

        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_confirm(navigator, backend.firmware.device, test_name + "_3")

        signature: bytes = renec.get_async_response().data
        verify_signature(from_public_key, message, signature)


    def test_ledger_sign_offchain_message_utf8_expert_refused(self, backend, navigator, test_name):
        enable_blind_signing(navigator, backend.firmware.device, test_name + "_1")
        enable_expert_mode(navigator, backend.firmware.device, test_name + "_2")

        renec = RenecClient(backend)
        from_public_key = renec.get_public_key(RENEC_PACKED_DERIVATION_PATH)

        offchain_message: OffchainMessage = OffchainMessage(0, bytes("Тестовое сообщение", 'utf-8'))
        message: bytes = offchain_message.serialize()

        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        with renec.send_async_sign_offchain_message(RENEC_PACKED_DERIVATION_PATH, message):
            navigation_helper_reject(navigator, backend.firmware.device, test_name + "_3")

        rapdu: RAPDU = renec.get_async_response()
        assert rapdu.status == ErrorType.USER_CANCEL

