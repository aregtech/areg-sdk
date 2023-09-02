13_pubclients_OUTPUT_OBJ := $(13_pubmesh_OUTPUT_OBJ)/pubclients

13_pubclients_SRC  :=
13_pubclients_OBJS :=

include $(13_pubmesh_BASE)/pubclients/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_pubclients_SRC), $(eval $(call obj, $(cpp), $(13_pubclients_OUTPUT_OBJ), 13_pubclients_OBJS, $(13_pubmesh_CXXFLAGS))))

DEPS = $(13_pubclients_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(13_pubclients_TARGET_BIN): $(areg_TARGET_PATH) $(13_pubclients_OBJS) $(13_pubmesh_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(13_pubmesh_CXXFLAGS) $(13_pubclients_OBJS) -o $@ $(13_pubmesh_LDFLAGS)
