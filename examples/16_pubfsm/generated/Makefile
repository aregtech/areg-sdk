16_generated_PROJECT_NAME := 16_generated

16_generated_BASE := $(16_pubfsm_BASE)/generated

16_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(16_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
16_generated_OUTPUT_OBJ := $(16_pubfsm_OUTPUT_OBJ)/$(16_generated_PROJECT_NAME)

16_generated_SRC  :=
16_generated_OBJS :=

include $(16_generated_BASE)/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(16_generated_SRC), $(eval $(call obj, $(cpp), $(16_generated_OUTPUT_OBJ), 16_generated_OBJS, $(16_pubfsm_CXXFLAGS))))

$(AREG_OUTPUT_LIB)/$(16_generated_TARGET_BIN): $(16_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
