16_pubfsm_BASE       := $(AREG_EXAMPLES)/16_pubfsm
16_pubfsm_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/16_pubfsm

# Project and binary names
16_generated_PROJECT_NAME	:= 16_generated
16_generated_TARGET_BIN		:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(16_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

16_pubclient_PROJECT_NAME	:= 16_pubclient
16_pubclient_TARGET_BIN		:= $(AREG_OUTPUT_BIN)/$(16_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)

16_pubservice_PROJECT_NAME	:= 16_pubservice
16_pubservice_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(16_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
16_pubfsm_CXXFLAGS	= -I$(16_pubfsm_BASE) $(examples_CXXFLAGS)
16_pubfsm_LDFLAGS   = $(AREG_LIB_INCLUDES) -lareg -l$(16_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
16_pubfsm_DEPEND	= $(AREG_DEPEND) $(16_generated_TARGET_BIN)

# 'generated' have to be first
include $(16_pubfsm_BASE)/generated/Makefile
include $(16_pubfsm_BASE)/pubservice/Makefile
include $(16_pubfsm_BASE)/pubclient/Makefile

16_pubfsm: $(16_generated_TARGET_BIN) $(16_pubservice_TARGET_BIN) $(16_pubclient_TARGET_BIN)

.PHONY: 16_pubfsm
