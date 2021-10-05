11_locmesh_BASE       := $(AREG_EXAMPLES)/11_locmesh
11_locmesh_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/11_locmesh

11_locmesh_CXXFLAGS   = -I$(11_locmesh_BASE) $(examples_CXXFLAGS)
11_locmesh_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(11_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(11_locmesh_BASE)/generated/Makefile
include $(11_locmesh_BASE)/locsvcmesh/Makefile

11_locmesh: $(AREG_OUTPUT_BIN)/$(11_locsvcmesh_TARGET_BIN)

.PHONY: 11_locmesh
