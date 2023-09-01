21_pubunblock_BASE       := $(AREG_EXAMPLES)/21_pubunblock
21_pubunblock_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/21_pubunblock

# Project and binary names
21_generated_PROJECT_NAME	:= 21_generated
21_generated_TARGET_BIN		:= $(AREG_LIB_PREFIX)$(21_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

21_pubclient_PROJECT_NAME	:= 21_pubclient
21_pubclient_TARGET_BIN		:= $(21_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)

21_pubservice_PROJECT_NAME	:= 21_pubservice
21_pubservice_TARGET_BIN	:= $(21_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
21_pubunblock_CXXFLAGS	= -I$(21_pubunblock_BASE) $(examples_CXXFLAGS)
21_pubunblock_LDFLAGS   = $(AREG_LIB_INCLUDES) -lareg -l$(21_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
21_pubunblock_DEPEND	= $(AREG_DEPEND) $(AREG_OUTPUT_LIB)/$(21_generated_TARGET_BIN)

# 'generated' have to be first
include $(21_pubunblock_BASE)/generated/Makefile
include $(21_pubunblock_BASE)/pubservice/Makefile
include $(21_pubunblock_BASE)/pubclient/Makefile

21_pubunblock:	\
	$(AREG_OUTPUT_LIB)/$(21_generated_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(21_pubclient_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(21_pubservice_TARGET_BIN)

.PHONY: 21_pubunblock
