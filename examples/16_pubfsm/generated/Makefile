16_generated_OUTPUT_OBJ := $(16_pubfsm_OUTPUT_OBJ)/generated

16_generated_SRC  :=
16_generated_OBJS :=

include $(16_pubfsm_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(16_generated_SRC), $(eval $(call obj, $(cpp), $(16_generated_OUTPUT_OBJ), 16_generated_OBJS, $(16_pubfsm_CXXFLAGS))))

DEPS = $(16_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(16_generated_TARGET_BIN): $(16_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
