# Multi pub/sub instance example project.

24_pubsubmulti_BASE       := $(AREG_EXAMPLES)/24_pubsubmulti
24_pubsubmulti_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/24_pubsubmulti

# Project and binary names
24_generated_PROJECT_NAME		:= 24_generated
24_generated_TARGET_BIN			:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(24_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

24_publisher_PROJECT_NAME		:= 24_publisher
24_publisher_TARGET_BIN			:= $(AREG_OUTPUT_BIN)/$(24_publisher_PROJECT_NAME)$(AREG_BIN_EXT)

24_subscribermulti_PROJECT_NAME	:= 24_subscribermulti
24_subscribermulti_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(24_subscribermulti_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
24_pubsubmulti_CXXFLAGS   = -I$(24_pubsubmulti_BASE) $(examples_CXXFLAGS)
24_pubsubmulti_LDFLAGS    = $(AREG_LIB_INCLUDES) -lareg -l$(24_generated_PROJECT_NAME) -l$(extend_PROJECT_NAME) $(LDFLAGS) $(AREG_EXTENDED_LIBS)

# Set build dependencies
24_pubsubmulti_DEPEND	= $(AREG_DEPEND) $(24_generated_TARGET_BIN)

# 'generated' have to be first
include $(24_pubsubmulti_BASE)/generated/Makefile
include $(24_pubsubmulti_BASE)/publisher/Makefile
include $(24_pubsubmulti_BASE)/subscribermulti/Makefile

24_pubsubmulti: $(24_generated_TARGET_BIN) $(24_publisher_TARGET_BIN) $(24_subscribermulti_TARGET_BIN)

.PHONY: 24_pubsubmulti
