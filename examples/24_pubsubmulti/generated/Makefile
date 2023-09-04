24_generated_OUTPUT_OBJ := $(24_pubsubmulti_OUTPUT_OBJ)/generated

24_generated_SRC  :=
24_generated_OBJS :=

include $(24_pubsubmulti_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(24_generated_SRC), $(eval $(call obj, $(cpp), $(24_generated_OUTPUT_OBJ), 24_generated_OBJS, $(24_pubsubmulti_CXXFLAGS))))

DEPS = $(24_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(24_generated_TARGET_BIN): $(24_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
