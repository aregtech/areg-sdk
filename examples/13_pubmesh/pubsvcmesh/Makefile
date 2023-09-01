13_pubsvcmesh_OUTPUT_OBJ	:= $(13_pubmesh_OUTPUT_OBJ)/pubsvcmesh

13_pubsvcmesh_SRC  :=
13_pubsvcmesh_OBJS :=

include $(13_pubmesh_BASE)/pubsvcmesh/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_pubsvcmesh_SRC), $(eval $(call obj, $(cpp), $(13_pubsvcmesh_OUTPUT_OBJ), 13_pubsvcmesh_OBJS, $(13_pubmesh_CXXFLAGS))))

DEPS = $(13_pubsvcmesh_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(13_pubsvcmesh_TARGET_BIN): $(areg_TARGET_PATH) $(13_pubsvcmesh_OBJS) $(13_pubmesh_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(13_pubmesh_CXXFLAGS) $(13_pubsvcmesh_OBJS) -o $@ $(13_pubmesh_LDFLAGS)
