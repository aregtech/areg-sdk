11_locsvcmesh_PROJECT_NAME := 11_locsvcmesh

11_locsvcmesh_TARGET_BIN := $(11_locsvcmesh_PROJECT_NAME)$(AREG_BIN_EXT)
11_locsvcmesh_OUTPUT_OBJ := $(11_locmesh_OUTPUT_OBJ)/$(11_locsvcmesh_PROJECT_NAME)

11_locsvcmesh_SRC  :=
11_locsvcmesh_OBJS :=

include $(11_locmesh_BASE)/locsvcmesh/src/Makefile

# define one target for each source file
$(foreach cpp, $(11_locsvcmesh_SRC), $(eval $(call obj, $(cpp), $(11_locsvcmesh_OUTPUT_OBJ), 11_locsvcmesh_OBJS, $(11_locmesh_CXXFLAGS))))

DEPS = $(11_locsvcmesh_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(11_locsvcmesh_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(11_generated_TARGET_BIN) $(11_locsvcmesh_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(11_locmesh_CXXFLAGS) $(11_locsvcmesh_OBJS) $(11_locmesh_LDFLAGS) -o $@
