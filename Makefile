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

APPNAME = RENEC
APPVERSION_M=1
APPVERSION_N=0
APPVERSION_P=0
APPVERSION=$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)

ifeq ($(TARGET_NAME), TARGET_NANOS)
APP_LOAD_PARAMS = --appFlags 0x800 # APPLICATION_FLAG_LIBRARY
else
APP_LOAD_PARAMS = --appFlags 0xa00 # APPLICATION_FLAG_LIBRARY + APPLICATION_FLAG_BOLOS_SETTINGS
endif
APP_LOAD_PARAMS += --curve ed25519
APP_LOAD_PARAMS += --path "44'/501'"
APP_LOAD_PARAMS += $(COMMON_LOAD_PARAMS)

ifeq ($(TARGET_NAME),TARGET_NANOS)
	ICONNAME=icons/nanos_app_renec.gif
else ifeq ($(TARGET_NAME),TARGET_NANOSP)
	ICONNAME=icons/nanosp_app_renec.gif
else ifeq ($(TARGET_NAME),TARGET_STAX)
	ICONNAME=icons/stax_app_renec.gif
else
	ICONNAME=icons/nanox_app_renec.gif
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

## USB U2F
WITH_U2F=0
ifneq ($(WITH_U2F),0)
	DEFINES += HAVE_U2F HAVE_IO_U2F U2F_PROXY_MAGIC=\"RENEC\" USB_SEGMENT_SIZE=64 BLE_SEGMENT_SIZE=32
endif

## Web USB
DEFINES += HAVE_WEBUSB WEBUSB_URL_SIZE_B=0 WEBUSB_URL=""

## Bluetooth
ifeq ($(TARGET_NAME),$(filter $(TARGET_NAME),TARGET_NANOX TARGET_STAX))
	DEFINES += HAVE_BLE BLE_COMMAND_TIMEOUT_MS=2000 HAVE_BLE_APDU
endif

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
	ifneq ($(TARGET_NAME),TARGET_NANOS)
		DEFINES += HAVE_GLO096
		DEFINES += BAGL_WIDTH=128 BAGL_HEIGHT=64
		DEFINES += HAVE_BAGL_ELLIPSIS
		DEFINES += HAVE_BAGL_FONT_OPEN_SANS_REGULAR_11PX
		DEFINES += HAVE_BAGL_FONT_OPEN_SANS_EXTRABOLD_11PX
		DEFINES += HAVE_BAGL_FONT_OPEN_SANS_LIGHT_16PX
	endif
endif

DEFINES += RESET_ON_CRASH

## Use development build
# DEBUG = 1

# Enable debug PRINTF
DEBUG = 0
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

##############
#  Compiler  #
##############

ifneq ($(BOLOS_ENV),)
	$(info BOLOS_ENV=$(BOLOS_ENV))
	CLANGPATH := $(BOLOS_ENV)/clang-arm-fropi/bin/
	GCCPATH := $(BOLOS_ENV)/gcc-arm-none-eabi-5_3-2016q1/bin/
else
$(info BOLOS_ENV is not set: falling back to CLANGPATH and GCCPATH)
endif
ifeq ($(CLANGPATH),)
$(info CLANGPATH is not set: clang will be used from PATH)
endif
ifeq ($(GCCPATH),)
$(info GCCPATH is not set: arm-none-eabi-* will be used from PATH)
endif

WERROR := -Werror=return-type -Werror=parentheses -Werror=format-security
CC := $(CLANGPATH)clang
CFLAGS += -O3 -Os -std=gnu99 -Wall -Wextra -Wuninitialized -Wshadow -Wformat=2 -Wwrite-strings -Wundef -fno-common $(WERROR)
AS := $(GCCPATH)arm-none-eabi-gcc
LD := $(GCCPATH)arm-none-eabi-gcc
LDFLAGS += -O3 -Os -Wall
LDLIBS += -lm -lgcc -lc

##################
#  Dependencies  #
##################

# import rules to compile glyphs
include $(BOLOS_SDK)/Makefile.glyphs

### computed variables
APP_SOURCE_PATH += src

# import SDK source paths per target (https://github.com/LedgerHQ/ledger-secure-sdk)
SDK_SOURCE_PATH += lib_stusb lib_stusb_impl
ifneq ($(WITH_U2F),0)
	SDK_SOURCE_PATH += lib_u2f
endif
ifneq ($(TARGET_NAME),TARGET_STAX)
	SDK_SOURCE_PATH += lib_ux
endif
ifeq ($(TARGET_NAME),$(filter $(TARGET_NAME),TARGET_NANOX TARGET_STAX))
	SDK_SOURCE_PATH += lib_blewbxx lib_blewbxx_impl
endif

# import custom libs
WITH_LIBRENEC=1
ifneq ($(WITH_LIBRENEC),0)
	SOURCE_FILES += $(filter-out %_test.c,$(wildcard librenec/*.c))
	CFLAGS += -Ilibrenec/include
	DEFINES += HAVE_SNPRINTF_FORMAT_U
	DEFINES += NDEBUG
endif

load: all load-only
load-only:
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

load-offline: all
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS) --offline

delete:
	python3 -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

include $(BOLOS_SDK)/Makefile.rules

dep/%.d: %.c Makefile

listvariants:
	@echo VARIANTS COIN renec