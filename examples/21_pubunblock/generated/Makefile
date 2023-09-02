21_generated_OUTPUT_OBJ := $(21_pubunblock_OUTPUT_OBJ)/generated

21_generated_SRC  :=
21_generated_OBJS :=

include $(21_pubunblock_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(21_generated_SRC), $(eval $(call obj, $(cpp), $(21_generated_OUTPUT_OBJ), 21_generated_OBJS, $(21_pubunblock_CXXFLAGS))))

DEPS = $(21_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(21_generated_TARGET_BIN): $(21_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
