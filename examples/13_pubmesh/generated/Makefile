13_generated_OUTPUT_OBJ := $(13_pubmesh_OUTPUT_OBJ)/generated

13_generated_SRC  :=
13_generated_OBJS :=

include $(13_pubmesh_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(13_generated_SRC), $(eval $(call obj, $(cpp), $(13_generated_OUTPUT_OBJ), 13_generated_OBJS, $(13_pubmesh_CXXFLAGS))))

DEPS = $(13_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(13_generated_TARGET_BIN): $(13_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
