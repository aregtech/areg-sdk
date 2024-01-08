23_pubsubctrl_OUTPUT_OBJ := $(23_pubsubmix_OUTPUT_OBJ)/pubsubctrl

23_pubsubctrl_SRC  :=
23_pubsubctrl_OBJS :=

include $(23_pubsubmix_BASE)/pubsubctrl/src/Makefile

# define one target for each source file
$(foreach cpp, $(23_pubsubctrl_SRC), $(eval $(call obj, $(cpp), $(23_pubsubctrl_OUTPUT_OBJ), 23_pubsubctrl_OBJS, $(23_pubsubmix_CXXFLAGS))))

DEPS = $(23_pubsubctrl_OBJS:%.o=%.d)
-include $(DEPS)

$(23_pubsubctrl_TARGET_BIN): $(areg_TARGET_BIN) $(23_pubsubctrl_OBJS) $(23_pubsubmix_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(23_pubsubmix_CXXFLAGS) $(23_pubsubctrl_OBJS) -o $@ $(23_pubsubmix_LDFLAGS)
