10_locsvc_BASE			:= $(AREG_EXAMPLES)/10_locsvc
10_locsvc_OUTPUT_OBJ	:= $(AREG_OUTPUT_OBJ)/10_locsvc

# Project names and locations
10_generated_PROJECT_NAME	:= 10_generated
10_generated_TARGET_BIN		:= $(AREG_LIB_PREFIX)$(10_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

10_locservice_PROJECT_NAME	:= 10_locservice
10_locservice_TARGET_BIN	:= $(10_locservice_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
10_locsvc_CXXFLAGS		= -I$(10_locsvc_BASE) $(examples_CXXFLAGS)
10_locsvc_LDFLAGS		= $(AREG_LIB_INCLUDES) -lareg -l$(10_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
10_locsvc_DEPEND		:= $(AREG_DEPEND) $(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN)

# 'generated' have to be first
include $(10_locsvc_BASE)/generated/Makefile
include $(10_locsvc_BASE)/locservice/Makefile

10_locsvc: \
	$(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(10_locservice_TARGET_BIN)

.PHONY: 10_locsvc
