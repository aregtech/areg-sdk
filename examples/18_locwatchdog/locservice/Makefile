18_locwatchdog_PROJECT_NAME := 18_locwatchdog

18_locwatchdog_TARGET_BIN := $(18_locwatchdog_PROJECT_NAME)$(AREG_BIN_EXT)
18_locwatchdog_OUTPUT_OBJ := $(18_locwatchdog_OUTPUT_OBJ)/$(18_locwatchdog_PROJECT_NAME)

18_locwatchdog_SRC  :=
18_locwatchdog_OBJS :=

include $(18_locwatchdog_BASE)/locservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(18_locwatchdog_SRC), $(eval $(call obj, $(cpp), $(18_locwatchdog_OUTPUT_OBJ), 18_locwatchdog_OBJS, $(18_locwatchdog_CXXFLAGS))))

DEPS = $(18_locwatchdog_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(18_locwatchdog_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(18_generated_TARGET_BIN) $(18_locwatchdog_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(18_locwatchdog_CXXFLAGS) $(18_locwatchdog_OBJS) $(18_locwatchdog_LDFLAGS) -o $@
