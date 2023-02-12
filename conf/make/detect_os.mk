# ###########################################################################
# This file is part of AREG SDK
# Copyright (c) 2017-2023, Aregtech UG
# Contact: info[at]aregtech.com
# ###########################################################################

# ###########################################################################
# This file detects the OS
# ###########################################################################

# default OS, platform and bitness
DETECTED_OS       := $(OS)
DETECTED_PROCESSOR:= $(PROCESSOR_ARCHITECTURE)
DETECTED_BITNESS  := $(shell getconf LONG_BIT)

DETECTED_OS := $(shell uname 2>/dev/null || echo Linux)
DETECTED_OS := $(patsubst CYGWIN%,Cygwin,$(DETECTED_OS))
DETECTED_OS := $(patsubst MSYS%,MSYS,$(DETECTED_OS))
DETECTED_OS := $(patsubst MINGW%,MSYS,$(DETECTED_OS))

UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P), unknown)
	ifeq ($(DETECTED_PROCESSOR),)
		UNAME_P := $(shell uname -m)
	endif
endif

ifeq ($(DETECTED_PROCESSOR),AMD64)
	DETECTED_PROCESSOR	:= x86_64
else ifeq ($(UNAME_P),x86_64)
	DETECTED_PROCESSOR	:= x86_64
else ifeq($(UNAME_P),aarch64)
	DETECTED_PROCESSOR	:= aarch64
else ifneq ($(filter %86,$(UNAME_P)),)
	DETECTED_PROCESSOR	:= x86
else ifneq ($(filter arm%,$(UNAME_P)),)
	DETECTED_PROCESSOR	:= arm
endif

$(info ********************************************)
$(info Detected OS is '$(DETECTED_OS)')
$(info Detected platform is '$(DETECTED_PROCESSOR)')
$(info Detected bitness is '$(DETECTED_BITNESS)')
$(info ********************************************)
