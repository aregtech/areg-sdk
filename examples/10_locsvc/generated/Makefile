10_generated_PROJECT_NAME := 10_generated

10_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(10_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
10_generated_OUTPUT_OBJ := $(10_locsvc_OUTPUT_OBJ)/generated

10_generated_SRC  :=
10_generated_OBJS :=

include $(10_locsvc_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(10_generated_SRC), $(eval $(call obj, $(cpp), $(10_generated_OUTPUT_OBJ), 10_generated_OBJS, $(10_locsvc_CXXFLAGS))))

DEPS = $(10_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN): $(10_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
