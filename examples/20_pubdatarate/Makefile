20_pubdatarate_BASE       := $(AREG_EXAMPLES)/20_pubdatarate
20_pubdatarate_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/20_pubdatarate

# Project and binary names
20_generated_PROJECT_NAME	:= 20_generated
20_generated_TARGET_BIN		:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(20_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

20_pubclient_PROJECT_NAME	:= 20_pubclient
20_pubclient_TARGET_BIN		:= $(AREG_OUTPUT_BIN)/$(20_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)

20_pubservice_PROJECT_NAME	:= 20_pubservice
20_pubservice_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(20_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
20_pubdatarate_CXXFLAGS	= -I$(20_pubdatarate_BASE) $(examples_CXXFLAGS)
20_pubdatarate_LDFLAGS  = $(AREG_LIB_INCLUDES)  -lareg -l$(20_generated_PROJECT_NAME) -lareg-extend $(LDFLAGS) $(AREG_EXTENDED_LIBS)

# Set build dependencies
20_pubdatarate_DEPEND	:= $(AREG_DEPEND) $(20_generated_TARGET_BIN)

# 'generated' have to be first
include $(20_pubdatarate_BASE)/generated/Makefile
include $(20_pubdatarate_BASE)/pubservice/Makefile
include $(20_pubdatarate_BASE)/pubclient/Makefile

20_pubdatarate: $(20_generated_TARGET_BIN) $(20_pubclient_TARGET_BIN) $(20_pubservice_TARGET_BIN)

.PHONY: 20_pubdatarate
