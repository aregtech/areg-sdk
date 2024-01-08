13_pubmesh_BASE			:= $(AREG_EXAMPLES)/13_pubmesh
13_pubmesh_OUTPUT_OBJ	:= $(AREG_OUTPUT_OBJ)/13_pubmesh

# Project and binary names
13_generated_PROJECT_NAME	:= 13_generated
13_generated_TARGET_BIN		:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(13_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

13_common_PROJECT_NAME	    := 13_common
13_common_TARGET_BIN	    := $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(13_common_PROJECT_NAME)$(AREG_STATIC_LIB)

13_pubclients_PROJECT_NAME	:= 13_pubclients
13_pubclients_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(13_pubclients_PROJECT_NAME)$(AREG_BIN_EXT)

13_pubservice_PROJECT_NAME	:= 13_pubservice
13_pubservice_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(13_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)

13_pubsvcmesh_PROJECT_NAME	:= 13_pubsvcmesh
13_pubsvcmesh_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(13_pubsvcmesh_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
13_pubmesh_CXXFLAGS		= -I$(13_pubmesh_BASE) $(examples_CXXFLAGS)
13_pubmesh_LDFLAGS		= $(AREG_LIB_INCLUDES) -lareg -l$(13_common_PROJECT_NAME) -l$(13_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
13_pubmesh_DEPEND		:= $(AREG_DEPEND) $(13_generated_TARGET_BIN) $(13_common_TARGET_BIN)

# 'generated' have to be first
include $(13_pubmesh_BASE)/generated/Makefile
include $(13_pubmesh_BASE)/common/Makefile
include $(13_pubmesh_BASE)/pubclients/Makefile
include $(13_pubmesh_BASE)/pubservice/Makefile
include $(13_pubmesh_BASE)/pubsvcmesh/Makefile

13_pubmesh: $(13_generated_TARGET_BIN) $(13_common_TARGET_BIN) $(13_pubclients_TARGET_BIN) $(13_pubservice_TARGET_BIN) $(13_pubsvcmesh_TARGET_BIN)

.PHONY: 13_pubmesh
