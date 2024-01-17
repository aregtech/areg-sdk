23_pubsubdyn_OUTPUT_OBJ := $(23_pubsubmix_OUTPUT_OBJ)/pubsubdyn

23_pubsubdyn_SRC  :=
23_pubsubdyn_OBJS :=

include $(23_pubsubmix_BASE)/pubsubdyn/src/Makefile

# define one target for each source file
$(foreach cpp, $(23_pubsubdyn_SRC), $(eval $(call obj, $(cpp), $(23_pubsubdyn_OUTPUT_OBJ), 23_pubsubdyn_OBJS, $(23_pubsubmix_CXXFLAGS))))

DEPS = $(23_pubsubdyn_OBJS:%.o=%.d)
-include $(DEPS)

$(23_pubsubdyn_TARGET_BIN): $(areg_TARGET_BIN) $(23_pubsubdyn_OBJS) $(23_pubsubmix_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(23_pubsubmix_CXXFLAGS) $(23_pubsubdyn_OBJS) -o $@ $(23_pubsubmix_LDFLAGS)
