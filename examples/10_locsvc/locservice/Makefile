10_locservice_PROJECT_NAME := 10_locservice

10_locservice_TARGET_BIN := $(10_locservice_PROJECT_NAME)$(AREG_BIN_EXT)
10_locservice_OUTPUT_OBJ := $(10_locsvc_OUTPUT_OBJ)/$(10_locservice_PROJECT_NAME)

10_locservice_SRC  :=
10_locservice_OBJS :=

include $(10_locsvc_BASE)/locservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(10_locservice_SRC), $(eval $(call obj, $(cpp), $(10_locservice_OUTPUT_OBJ), 10_locservice_OBJS, $(10_locsvc_CXXFLAGS))))

DEPS = $(10_locservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(10_locservice_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN) $(10_locservice_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(10_locsvc_CXXFLAGS) $(10_locservice_OBJS) $(10_locsvc_LDFLAGS) -o $@
