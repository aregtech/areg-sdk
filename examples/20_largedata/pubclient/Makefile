19_pubclient_PROJECT_NAME := 19_pubclientwdog

19_pubclient_TARGET_BIN := $(19_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)
19_pubclient_OUTPUT_OBJ := $(19_pubwatchdog_OUTPUT_OBJ)/pubclient

19_pubclient_SRC  :=
19_pubclient_OBJS :=

include $(19_pubwatchdog_BASE)/pubclient/src/Makefile

# define one target for each source file
$(foreach cpp, $(19_pubclient_SRC), $(eval $(call obj, $(cpp), $(19_pubclient_OUTPUT_OBJ), 19_pubclient_OBJS, $(19_pubwatchdog_CXXFLAGS))))

DEPS = $(19_pubclient_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(19_pubclient_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(19_generated_TARGET_BIN) $(19_pubclient_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(19_pubwatchdog_CXXFLAGS) $(19_pubclient_OBJS) $(19_pubwatchdog_LDFLAGS) -o $@
