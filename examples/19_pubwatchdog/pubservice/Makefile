19_pubservice_PROJECT_NAME := 19_pubservicewdog

19_pubservice_TARGET_BIN := $(19_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)
19_pubservice_OUTPUT_OBJ := $(19_pubwatchdog_OUTPUT_OBJ)/pubservice

19_pubservice_SRC  :=
19_pubservice_OBJS :=

include $(19_pubwatchdog_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(19_pubservice_SRC), $(eval $(call obj, $(cpp), $(19_pubservice_OUTPUT_OBJ), 19_pubservice_OBJS, $(19_pubwatchdog_CXXFLAGS))))

DEPS = $(19_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(19_pubservice_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(19_generated_TARGET_BIN) $(19_pubservice_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(19_pubwatchdog_CXXFLAGS) $(19_pubservice_OBJS) $(19_pubwatchdog_LDFLAGS) -o $@
