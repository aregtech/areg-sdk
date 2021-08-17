14_ipclightctrl_PROJECT_NAME := 14_ipclightctrl

14_ipclightctrl_BASE := $(14_pubsvclight_BASE)/ipclightctrl

14_ipclightctrl_TARGET_BIN := $(14_ipclightctrl_PROJECT_NAME)$(AREG_BIN_EXT)
14_ipclightctrl_OUTPUT_OBJ := $(14_pubsvclight_OUTPUT_OBJ)/$(14_ipclightctrl_PROJECT_NAME)

14_ipclightctrl_SRC  :=
14_ipclightctrl_OBJS :=

include $(14_ipclightctrl_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(14_ipclightctrl_SRC), $(eval $(call obj, $(cpp), $(14_ipclightctrl_OUTPUT_OBJ), 14_ipclightctrl_OBJS, $(14_pubsvclight_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(14_ipclightctrl_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(14_generated_TARGET_BIN) $(14_ipclightctrl_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(14_pubsvclight_CXXFLAGS) $(14_ipclightctrl_OBJS) $(14_pubsvclight_LDFLAGS) -o $@
