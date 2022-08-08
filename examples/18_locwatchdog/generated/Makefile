18_generated_PROJECT_NAME := 18_generated

18_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(18_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
18_generated_OUTPUT_OBJ := $(18_locsvc_OUTPUT_OBJ)/$(18_generated_PROJECT_NAME)

18_generated_SRC  :=
18_generated_OBJS :=

include $(18_locwatchdog_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(18_generated_SRC), $(eval $(call obj, $(cpp), $(18_generated_OUTPUT_OBJ), 18_generated_OBJS, $(18_locwatchdog_CXXFLAGS))))

DEPS = $(18_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(18_generated_TARGET_BIN): $(18_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
