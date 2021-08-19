10_generated_PROJECT_NAME := 10_generated

10_generated_BASE := $(10_locsvchello_BASE)/generated

10_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(10_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
10_generated_OUTPUT_OBJ := $(10_locsvchello_OUTPUT_OBJ)/$(10_generated_PROJECT_NAME)

10_generated_SRC  :=
10_generated_OBJS :=

include $(10_generated_BASE)/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(10_generated_SRC), $(eval $(call obj, $(cpp), $(10_generated_OUTPUT_OBJ), 10_generated_OBJS, $(10_locsvchello_CXXFLAGS))))

$(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN): $(10_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
