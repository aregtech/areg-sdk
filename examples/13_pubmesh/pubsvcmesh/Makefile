# example project 13_pubmesh -> 13_pubsvcmesh

13_pubsvcmesh_PROJECT_NAME := 13_pubsvcmesh

13_pubsvcmesh_BASE := $(13_pubmesh_BASE)/pubsvcmesh

13_pubsvcmesh_TARGET_BIN := $(13_pubsvcmesh_PROJECT_NAME)$(AREG_BIN_EXT)
13_pubsvcmesh_OUTPUT_OBJ := $(13_pubmesh_OUTPUT_OBJ)/$(13_pubsvcmesh_PROJECT_NAME)

13_pubsvcmesh_SRC  :=
13_pubsvcmesh_OBJS :=

include $(13_pubsvcmesh_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_pubsvcmesh_SRC), $(eval $(call obj, $(cpp), $(13_pubsvcmesh_OUTPUT_OBJ), 13_pubsvcmesh_OBJS, $(13_pubmesh_CXXFLAGS))))

DEPS = $(13_pubsvcmesh_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(13_pubsvcmesh_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(13_generated_TARGET_BIN) $(13_pubsvcmesh_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(13_pubmesh_CXXFLAGS) $(13_pubsvcmesh_OBJS) $(13_pubmesh_LDFLAGS) -o $@
