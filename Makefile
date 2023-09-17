# ****************************************************************************
#    Ledger App RENEC
#    (c) 2023 Ledger SAS.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ****************************************************************************

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

########################################
#        Mandatory configuration       #
########################################

# Application name
APPNAME = RENEC

# Application version
APPVERSION_M=0
APPVERSION_N=0
APPVERSION_P=1
APPVERSION=$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)

# Application source files
APP_SOURCE_PATH += src

# Application icons following guidelines:
# https://developers.ledger.com/docs/embedded-app/icons/
ifeq ($(TARGET_NAME),TARGET_NANOS)
	ICONNAME=icons/nanos_app_renec.gif
else ifeq ($(TARGET_NAME),TARGET_STAX)
	ICONNAME=icons/stax_app_renec.gif
else
	ICONNAME=icons/nanox_app_renec.gif
endif

# Application allowed derivation curves.
# Possibles curves are: secp256k1, secp256r1, ed25519 and bls12381g1
# If your app needs it, you can specify multiple curves by using:
# `CURVE_APP_LOAD_PARAMS = <curve1> <curve2>`
CURVE_APP_LOAD_PARAMS = ed25519

# Application allowed derivation paths.
# You should request a specific path for your app.
# This serve as an isolation mechanism.
# Most application will have to request a path according to the BIP-0044
# and SLIP-0044 standards.
# If your app needs it, you can specify multiple path by using:
# `PATH_APP_LOAD_PARAMS = "44'/1'" "45'/1'"`
PATH_APP_LOAD_PARAMS = "44'/501'"   # purpose=coin(44) / coin_type=Testnet(1)

APP_LOAD_PARAMS += $(COMMON_LOAD_PARAMS)

# Enabling DEBUG flag will enable PRINTF and disable optimizations
DEBUG = 1
ifneq ($(DEBUG),0)
	DEFINES += HAVE_PRINTF
	ifeq ($(TARGET_NAME),TARGET_NANOS)
		DEFINES += PRINTF=screen_printf
	else
		DEFINES += PRINTF=mcu_usb_printf
	endif
else
	DEFINES += PRINTF\(...\)=
endif

########################################
#     Application custom permissions   #
########################################

# See SDK `include/appflags.h` for the purpose of each permission
HAVE_APPLICATION_FLAG_LIBRARY = 1
ifneq ($(TARGET_NAME), TARGET_NANOS)
HAVE_APPLICATION_FLAG_BOLOS_SETTINGS = 1
endif

################
# Default rule #
################
all: default

############
# Platform #
############

DEFINES += $(DEFINES_LIB)
DEFINES += APPNAME=\"$(APPNAME)\"
DEFINES += APPVERSION=\"$(APPVERSION)\"
DEFINES += MAJOR_VERSION=$(APPVERSION_M) MINOR_VERSION=$(APPVERSION_N) PATCH_VERSION=$(APPVERSION_P)
DEFINES += OS_IO_SEPROXYHAL
DEFINES += HAVE_SPRINTF
DEFINES += UNUSED\(x\)=\(void\)x

## USB HID
DEFINES += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=6 IO_HID_EP_LENGTH=64 HAVE_USB_APDU
DEFINES += USB_SEGMENT_SIZE=64

## Web USB
DEFINES += HAVE_WEBUSB WEBUSB_URL_SIZE_B=0 WEBUSB_URL=""

## Protect stack overflows
DEFINES += HAVE_BOLOS_APP_STACK_CANARY
ifeq ($(TARGET_NAME),TARGET_NANOS)
	DEFINES += IO_SEPROXYHAL_BUFFER_SIZE_B=128
else
	DEFINES += IO_SEPROXYHAL_BUFFER_SIZE_B=300
endif

ifeq ($(TARGET_NAME),TARGET_STAX)
	DEFINES += NBGL_QRCODE
else
	DEFINES += HAVE_BAGL HAVE_UX_FLOW
endif

CC := $(CLANGPATH)clang
AS := $(GCCPATH)arm-none-eabi-gcc
LD := $(GCCPATH)arm-none-eabi-gcc
LDFLAGS += -O3 -Os

##################
#  Dependencies  #
##################

# import generic rules from SDK
include $(BOLOS_SDK)/Makefile.rules

# import rules to compile glyphs
include $(BOLOS_SDK)/Makefile.glyphs

# import SDK source paths per target (https://github.com/LedgerHQ/ledger-secure-sdk)
SDK_SOURCE_PATH += lib_stusb lib_stusb_impl
ifneq ($(TARGET_NAME),TARGET_STAX)
	SDK_SOURCE_PATH += lib_ux
endif

# add dependencies
dep/%.d: %.c Makefile
