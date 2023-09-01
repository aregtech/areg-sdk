16_pubservice_PROJECT_NAME := 16_pubservice

16_pubservice_TARGET_BIN := $(16_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)
16_pubservice_OUTPUT_OBJ := $(16_pubfsm_OUTPUT_OBJ)/pubservice

16_pubservice_SRC  :=
16_pubservice_OBJS :=

include $(16_pubfsm_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(16_pubservice_SRC), $(eval $(call obj, $(cpp), $(16_pubservice_OUTPUT_OBJ), 16_pubservice_OBJS, $(16_pubfsm_CXXFLAGS))))

DEPS = $(16_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(16_pubservice_TARGET_BIN): $(areg_TARGET_PATH) $(16_pubservice_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(16_pubfsm_CXXFLAGS) $(16_pubservice_OBJS) -o $@ $(16_pubfsm_LDFLAGS)
