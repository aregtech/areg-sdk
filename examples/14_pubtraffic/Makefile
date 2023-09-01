14_pubtraffic_BASE       := $(AREG_EXAMPLES)/14_pubtraffic
14_pubtraffic_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/14_pubtraffic

# Project and binary names
14_generated_PROJECT_NAME	:= 14_generated
14_generated_TARGET_BIN		:= $(AREG_LIB_PREFIX)$(14_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

14_pubclient_PROJECT_NAME	:= 14_pubclient
14_pubclient_TARGET_BIN		:= $(14_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)

14_pubservice_PROJECT_NAME := 14_pubservice
14_pubservice_TARGET_BIN := $(14_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
14_pubtraffic_CXXFLAGS   = -I$(14_pubtraffic_BASE) $(examples_CXXFLAGS)
14_pubtraffic_LDFLAGS    = $(AREG_LIB_INCLUDES) -lareg -l$(14_generated_PROJECT_NAME) -lareg-extend $(LDFLAGS) $(AREG_EXTENDED_LIBS)

# Set build dependencies
14_pubtraffic_DEPEND	= $(AREG_DEPEND) $(AREG_OUTPUT_LIB)/$(14_generated_TARGET_BIN)

# 'generated' have to be first
include $(14_pubtraffic_BASE)/generated/Makefile
include $(14_pubtraffic_BASE)/pubclient/Makefile
include $(14_pubtraffic_BASE)/pubservice/Makefile

14_pubtraffic:	\
	$(AREG_OUTPUT_LIB)/$(14_generated_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(14_pubclient_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(14_pubservice_TARGET_BIN)

.PHONY: 14_pubtraffic
