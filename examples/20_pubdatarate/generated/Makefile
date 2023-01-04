20_generated_PROJECT_NAME := 20_generated

20_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(20_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
20_generated_OUTPUT_OBJ := $(20_pubdatarate_OUTPUT_OBJ)/generated

20_generated_SRC  :=
20_generated_OBJS :=

include $(20_pubdatarate_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(20_generated_SRC), $(eval $(call obj, $(cpp), $(20_generated_OUTPUT_OBJ), 20_generated_OBJS, $(20_pubdatarate_CXXFLAGS))))

DEPS = $(20_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(20_generated_TARGET_BIN): $(20_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
