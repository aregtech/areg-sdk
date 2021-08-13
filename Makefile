# ###########################################################################
# This file is part of AREG IoT SDK
# Copyright (c) Aregtech UG, 2017-2021
# Contact: info[at]aregtech.com
# ###########################################################################

# AREG SDK root directory without '/'
MAKEFILE_ROOT       := $(abspath $(lastword $(MAKEFILE_LIST)))
AREG_SDK_ROOT       := $(patsubst %/,%,$(dir $(MAKEFILE_ROOT)))
MAKEFILE_CONFIG_DIR := $(AREG_SDK_ROOT)/conf/make
AREG_BASE           := $(AREG_SDK_ROOT)/framework
AREG_EXAMPLES       := $(AREG_SDK_ROOT)/examples

# 'all' must be defined first before include project's makefiles
all: framework examples

include $(MAKEFILE_CONFIG_DIR)/common.mk

include $(AREG_BASE)/Makefile
include $(AREG_EXAMPLES)/Makefile

.PHONY: all