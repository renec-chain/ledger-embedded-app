import logging

from ragger.navigator import Navigator, NavIns, NavInsID

from .renec_utils import ROOT_SCREENSHOT_PATH

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)


def enable_blind_signing(navigator: Navigator,
                         device_name: str,
                         snapshots_name: str):
    if device_name.startswith("nano"):
        nav = [NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.RIGHT_CLICK, 
               NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK]
    else:
        nav = [NavInsID.USE_CASE_HOME_SETTINGS,
               NavInsID.USE_CASE_SETTINGS_NEXT,
               NavIns(NavInsID.TOUCH, (348,132)),
               NavInsID.USE_CASE_SETTINGS_MULTI_PAGE_EXIT]
    navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                   snapshots_name,
                                   nav,
                                   screen_change_before_first_instruction=False)
    

def enable_short_public_key(navigator: Navigator,
                            device_name: str,
                            snapshots_name: str):
    if device_name.startswith("nano"):
        nav = [NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK]
    else:
        nav = [NavInsID.USE_CASE_HOME_SETTINGS,
               NavInsID.USE_CASE_SETTINGS_NEXT,
               NavIns(NavInsID.TOUCH, (348,251)),
               NavInsID.USE_CASE_SETTINGS_MULTI_PAGE_EXIT]
    navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                   snapshots_name,
                                   nav,
                                   screen_change_before_first_instruction=False)


def enable_expert_mode(navigator: Navigator, device_name: str, snapshots_name: str):
    if device_name.startswith("nano"):
        nav = [NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK,
               NavInsID.RIGHT_CLICK, NavInsID.BOTH_CLICK]
    else:
        nav = [NavInsID.USE_CASE_HOME_SETTINGS,
               NavInsID.USE_CASE_SETTINGS_NEXT,
               NavIns(NavInsID.TOUCH, (348,382)),
               NavInsID.USE_CASE_SETTINGS_MULTI_PAGE_EXIT]
    navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                   snapshots_name,
                                   nav,
                                   screen_change_before_first_instruction=False)    

def _navigation_helper(navigator: Navigator,
                       device_name: str,
                       accept: bool,
                       snapshots_name: str):
    if device_name.startswith("nano"):
        navigate_instruction = NavInsID.RIGHT_CLICK
        validation_instructions = [NavInsID.BOTH_CLICK]
        if accept:
            text = "Approve"
        else:
            text = "Reject"
    else:
        navigate_instruction = NavInsID.USE_CASE_REVIEW_TAP
        text = "Hold to sign"
        if accept:
            validation_instructions = [NavInsID.USE_CASE_REVIEW_CONFIRM, 
                                       NavInsID.USE_CASE_STATUS_DISMISS]
        else:
            validation_instructions = [NavInsID.USE_CASE_REVIEW_REJECT, 
                                       NavInsID.USE_CASE_CHOICE_CONFIRM, 
                                       NavInsID.USE_CASE_STATUS_DISMISS]


    navigator.navigate_until_text_and_compare(navigate_instruction,
                                              validation_instructions,
                                              text,
                                              ROOT_SCREENSHOT_PATH,
                                              snapshots_name)

def navigation_helper_confirm(navigator: Navigator, 
                              device_name: str, 
                              snapshots_name: str):
    _navigation_helper(navigator=navigator, 
                       device_name=device_name, 
                       accept=True, 
                       snapshots_name=snapshots_name)

def navigation_helper_reject(navigator: Navigator, 
                             device_name: str, 
                             snapshots_name: str):
    _navigation_helper(navigator=navigator, 
                       device_name=device_name, 
                       accept=False, 
                       snapshots_name=snapshots_name)
