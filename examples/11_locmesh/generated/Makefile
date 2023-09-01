11_generated_OUTPUT_OBJ := $(11_locmesh_OUTPUT_OBJ)/generated

11_generated_SRC  :=
11_generated_OBJS :=

include $(11_locmesh_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(11_generated_SRC), $(eval $(call obj, $(cpp), $(11_generated_OUTPUT_OBJ), 11_generated_OBJS, $(11_locmesh_CXXFLAGS))))

DEPS = $(11_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(11_generated_TARGET_BIN): $(11_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
