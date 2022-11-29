13_pubmesh_BASE       := $(AREG_EXAMPLES)/13_pubmesh
13_pubmesh_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/13_pubmesh

13_pubmesh_CXXFLAGS   = -I$(13_pubmesh_BASE) $(examples_CXXFLAGS)
13_pubmesh_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(13_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(13_pubmesh_BASE)/generated/Makefile
include $(13_pubmesh_BASE)/pubclients/Makefile
include $(13_pubmesh_BASE)/pubservice/Makefile
include $(13_pubmesh_BASE)/pubsvcmesh/Makefile

13_pubmesh: $(AREG_OUTPUT_BIN)/$(13_pubclients_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(13_pubservice_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(13_pubsvcmesh_TARGET_BIN)

.PHONY: 13_pubmesh
