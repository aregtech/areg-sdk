15_pubworker_BASE       := $(AREG_EXAMPLES)/15_pubworker
15_pubworker_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/15_pubworker

# Project and binary names
15_generated_PROJECT_NAME	:= 15_generated
15_generated_TARGET_BIN		:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(15_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

15_pubclient_PROJECT_NAME	:= 15_pubclient
15_pubclient_TARGET_BIN		:= $(AREG_OUTPUT_BIN)/$(15_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)

15_pubservice_PROJECT_NAME	:= 15_pubservice
15_pubservice_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(15_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
15_pubworker_CXXFLAGS   = -I$(15_pubworker_BASE) $(examples_CXXFLAGS)
15_pubworker_LDFLAGS    = $(AREG_LIB_INCLUDES) -lareg -l$(15_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
15_pubworker_DEPEND		= $(AREG_DEPEND) $(15_generated_TARGET_BIN)

# 'generated' have to be first
include $(15_pubworker_BASE)/generated/Makefile
include $(15_pubworker_BASE)/pubclient/Makefile
include $(15_pubworker_BASE)/pubservice/Makefile

15_pubworker: $(15_generated_TARGET_BIN) $(15_pubclient_TARGET_BIN) $(15_pubservice_TARGET_BIN)

.PHONY: 15_pubworker
