13_pubservice_OUTPUT_OBJ := $(13_pubmesh_OUTPUT_OBJ)/pubservice

13_pubservice_SRC  :=
13_pubservice_OBJS :=

include $(13_pubmesh_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_pubservice_SRC), $(eval $(call obj, $(cpp), $(13_pubservice_OUTPUT_OBJ), 13_pubservice_OBJS, $(13_pubmesh_CXXFLAGS))))

DEPS = $(13_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(13_pubservice_TARGET_BIN): $(areg_TARGET_PATH) $(13_pubservice_OBJS) $(13_pubmesh_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(13_pubmesh_CXXFLAGS) $(13_pubservice_OBJS) -o $@ $(13_pubmesh_LDFLAGS)
