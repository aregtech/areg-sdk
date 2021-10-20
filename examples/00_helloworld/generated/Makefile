00_generated_PROJECT_NAME := 00_generated

00_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(00_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
00_generated_OUTPUT_OBJ := $(00_helloworld_OUTPUT_OBJ)/$(00_generated_PROJECT_NAME)

00_generated_SRC  :=
00_generated_OBJS :=

include $(00_helloworld_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(00_generated_SRC), $(eval $(call obj, $(cpp), $(00_generated_OUTPUT_OBJ), 00_generated_OBJS, $(00_helloworld_CXXFLAGS))))

DEPS = $(00_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(00_generated_TARGET_BIN): $(00_generated_OBJS)
	@echo "$(00_helloworld_BASE)/generated/src/private/Makefile"
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
