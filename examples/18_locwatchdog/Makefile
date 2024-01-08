18_locwatchdog_BASE       := $(AREG_EXAMPLES)/18_locwatchdog
18_locwatchdog_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/18_locwatchdog

# Project and binary names
18_generated_PROJECT_NAME	:= 18_generated
18_generated_TARGET_BIN		:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(18_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

18_locwatchdog_PROJECT_NAME := 18_locservice
18_locservice_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(18_locwatchdog_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
18_locwatchdog_CXXFLAGS = -I$(18_locwatchdog_BASE) $(examples_CXXFLAGS)
18_locwatchdog_LDFLAGS	= $(AREG_LIB_INCLUDES) -l$(18_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# Set build dependencies
18_locwatchdog_DEPEND	= $(AREG_DEPEND) $(18_generated_TARGET_BIN)

# 'generated' have to be first
include $(18_locwatchdog_BASE)/generated/Makefile
include $(18_locwatchdog_BASE)/locservice/Makefile

18_locwatchdog: $(18_generated_TARGET_BIN) $(18_locservice_TARGET_BIN)

.PHONY: 18_locwatchdog
