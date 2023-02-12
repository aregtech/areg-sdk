# ###########################################################################
# This file is part of AREG SDK
# Copyright (c) 2017-2022, Aregtech UG
# Contact: info[at]aregtech.com
# ###########################################################################

# ###########################################################################
# This file detects the OS
# ###########################################################################

#default OS, platform and bitness
DETECTED_OS       := Linux
DETECTED_PROCESSOR:= $(PROCESSOR_ARCHITECTURE)
DETECTED_BITNESS  := 32

ifeq '$(findstring ;,$(PATH))' ';'

$(info it is in Windows)

	# detected windows
    DETECTED_OS := Windows
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		DETECTED_PROCESSOR:= x86_64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		DETECTED_PROCESSOR:= x86
	endif

else
	DETECTED_OS := $(shell uname 2>/dev/null || echo Linux)
    DETECTED_OS := $(patsubst CYGWIN%,Cygwin,$(DETECTED_OS))
    DETECTED_OS := $(patsubst MSYS%,MSYS,$(DETECTED_OS))
    DETECTED_OS := $(patsubst MINGW%,MSYS,$(DETECTED_OS))

	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P), unknown)
		UNAME_P := $(shell uname -m)
	endif

	ifeq ($(UNAME_P),x86_64)
		DETECTED_PROCESSOR	:= x86_64
		DETECTED_BITNESS 	:= 64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		DETECTED_PROCESSOR	:= x86
		DETECTED_BITNESS	:= 32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		DETECTED_PROCESSOR	:= arm
		DETECTED_BITNESS	:= 32
	endif

endif

$(info ********************************************)
$(info Detected OS is '$(DETECTED_OS)')
$(info Detected platform is '$(DETECTED_PROCESSOR)')
$(info Detected bitness is '$(DETECTED_BITNESS)')
$(info ********************************************)
