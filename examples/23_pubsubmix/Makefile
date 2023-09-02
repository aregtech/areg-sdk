23_pubsubmix_BASE       := $(AREG_EXAMPLES)/23_pubsubmix
23_pubsubmix_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/23_pubsubmix

# Project and binary names
23_common_PROJECT_NAME	:= 23_common
23_common_TARGET_BIN	:= $(AREG_LIB_PREFIX)$(23_common_PROJECT_NAME)$(AREG_STATIC_LIB)

23_generated_PROJECT_NAME	:= 23_generated
23_generated_TARGET_BIN		:= $(AREG_LIB_PREFIX)$(23_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

23_pubsubctrl_PROJECT_NAME	:= 23_pubsubctrl
23_pubsubctrl_TARGET_BIN	:= $(23_pubsubctrl_PROJECT_NAME)$(AREG_BIN_EXT)

23_pubsubdyn_PROJECT_NAME := 23_pubsubdyn
23_pubsubdyn_TARGET_BIN := $(23_pubsubdyn_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
23_pubsubmix_CXXFLAGS   = -I$(23_pubsubmix_BASE) $(examples_CXXFLAGS)
23_pubsubmix_LDFLAGS    = $(AREG_LIB_INCLUDES) -lareg -l$(23_common_PROJECT_NAME) -l$(23_generated_PROJECT_NAME) -l$(extend_PROJECT_NAME) $(LDFLAGS) $(AREG_EXTENDED_LIBS)

# Set build dependencies
23_pubsubmix_DEPEND		= $(AREG_DEPEND) $(AREG_OUTPUT_LIB)/$(23_generated_TARGET_BIN) $(AREG_OUTPUT_LIB)/$(23_common_TARGET_BIN)

# 'generated' have to be first
include $(23_pubsubmix_BASE)/generated/Makefile
include $(23_pubsubmix_BASE)/common/Makefile
include $(23_pubsubmix_BASE)/pubsubctrl/Makefile
include $(23_pubsubmix_BASE)/pubsubdyn/Makefile

23_pubsubmix:	\
	$(AREG_OUTPUT_LIB)/$(23_generated_TARGET_BIN)	\
	$(AREG_OUTPUT_LIB)/$(23_common_TARGET_BIN)		\
	$(AREG_OUTPUT_BIN)/$(23_pubsubctrl_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(23_pubsubdyn_TARGET_BIN)

.PHONY: 23_pubsubmix
