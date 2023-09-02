12_pubsvc_BASE       := $(AREG_EXAMPLES)/12_pubsvc
12_pubsvc_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/12_pubsvc

# Project and binary names
12_generated_PROJECT_NAME	:= 12_generated
12_generated_TARGET_BIN		:= $(AREG_LIB_PREFIX)$(12_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

12_pubclient_PROJECT_NAME	:= 12_pubclient
12_pubclient_TARGET_BIN		:= $(12_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)

12_pubservice_PROJECT_NAME	:= 12_pubservice
12_pubservice_TARGET_BIN	:= $(12_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
12_pubsvc_CXXFLAGS	= -I$(12_pubsvc_BASE) $(examples_CXXFLAGS)
12_pubsvc_LDFLAGS   = $(AREG_LIB_INCLUDES) -lareg -l$(12_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
12_pubsvc_DEPEND	:= $(AREG_DEPEND) $(AREG_OUTPUT_LIB)/$(12_generated_TARGET_BIN)

# 'generated' have to be first
include $(12_pubsvc_BASE)/generated/Makefile
include $(12_pubsvc_BASE)/pubservice/Makefile
include $(12_pubsvc_BASE)/pubclient/Makefile

12_pubsvc:	\
	$(AREG_OUTPUT_LIB)/$(12_generated_TARGET_BIN) 	\
	$(AREG_OUTPUT_BIN)/$(12_pubservice_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(12_pubclient_TARGET_BIN)

.PHONY: 12_pubsvc
