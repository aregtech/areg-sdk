11_locsvcmesh_OUTPUT_OBJ := $(11_locmesh_OUTPUT_OBJ)/locsvcmesh

11_locsvcmesh_SRC  :=
11_locsvcmesh_OBJS :=

include $(11_locmesh_BASE)/locsvcmesh/src/Makefile

# define one target for each source file
$(foreach cpp, $(11_locsvcmesh_SRC), $(eval $(call obj, $(cpp), $(11_locsvcmesh_OUTPUT_OBJ), 11_locsvcmesh_OBJS, $(11_locmesh_CXXFLAGS))))

DEPS = $(11_locsvcmesh_OBJS:%.o=%.d)
-include $(DEPS)

$(11_locsvcmesh_TARGET_BIN): $(areg_TARGET_BIN) $(11_locsvcmesh_OBJS) $(11_locmesh_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(11_locmesh_CXXFLAGS) $(11_locsvcmesh_OBJS) -o $@ $(11_locmesh_LDFLAGS)
