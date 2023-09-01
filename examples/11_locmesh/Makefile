11_locmesh_BASE       := $(AREG_EXAMPLES)/11_locmesh
11_locmesh_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/11_locmesh

# Project names and locations
11_generated_PROJECT_NAME	:= 11_generated
11_generated_TARGET_BIN		:= $(AREG_LIB_PREFIX)$(11_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

11_locsvcmesh_PROJECT_NAME	:= 11_locmesh
11_locsvcmesh_TARGET_BIN	:= $(11_locsvcmesh_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
11_locmesh_CXXFLAGS	= -I$(11_locmesh_BASE) $(examples_CXXFLAGS)
11_locmesh_LDFLAGS	= $(AREG_LIB_INCLUDES) -lareg -l$(11_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
11_locmesh_DEPEND	:= $(AREG_DEPEND) $(AREG_OUTPUT_LIB)/$(11_generated_TARGET_BIN)

# 'generated' have to be first
include $(11_locmesh_BASE)/generated/Makefile
include $(11_locmesh_BASE)/locsvcmesh/Makefile

11_locmesh:	\
	$(AREG_OUTPUT_LIB)/$(11_generated_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(11_locsvcmesh_TARGET_BIN)

.PHONY: 11_locmesh
