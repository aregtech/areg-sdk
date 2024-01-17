# ###########################################################################
# This file is part of AREG SDK
# Copyright (c) Aregtech UG, 2017-2023
# Contact: info[at]aregtech.com
# ###########################################################################

# AREG SDK root directory without '/'
MAKEFILE_ROOT       := $(abspath $(lastword $(MAKEFILE_LIST)))
AREG_SDK_ROOT       := $(patsubst %/,%,$(dir $(MAKEFILE_ROOT)))
AREG_MAKE_CONFIG_DIR:= $(AREG_SDK_ROOT)/conf/make
AREG_BASE           := $(AREG_SDK_ROOT)/framework
AREG_EXAMPLES       := $(AREG_SDK_ROOT)/examples
AREG_EXTEND         := $(AREG_BASE)/extend
AREG_THIRDPARTY     := $(AREG_SDK_ROOT)/thirdparty

DEPS_EXAMPLES :=
ifneq ($(AREG_BUILD_EXAMPLES),0)
DEPS_EXAMPLES := examples
endif

# 'all' must be defined first before include project's makefiles
all: thirdparty framework $(DEPS_EXAMPLES)

include $(AREG_MAKE_CONFIG_DIR)/detect_os.mk
include $(AREG_MAKE_CONFIG_DIR)/user.mk
include $(AREG_MAKE_CONFIG_DIR)/common.mk

include $(AREG_THIRDPARTY)/Makefile
include $(AREG_BASE)/Makefile

ifneq ($(AREG_BUILD_EXAMPLES),0)
include $(AREG_EXAMPLES)/Makefile
endif

.PHONY: all
