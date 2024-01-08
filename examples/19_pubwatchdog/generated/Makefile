19_generated_OUTPUT_OBJ := $(19_pubwatchdog_OUTPUT_OBJ)/generated

19_generated_SRC  :=
19_generated_OBJS :=

include $(19_pubwatchdog_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(19_generated_SRC), $(eval $(call obj, $(cpp), $(19_generated_OUTPUT_OBJ), 19_generated_OBJS, $(19_pubwatchdog_CXXFLAGS))))

DEPS = $(19_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(19_generated_TARGET_BIN): $(19_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
