23_generated_PROJECT_NAME := 23_generated

23_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(23_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
23_generated_OUTPUT_OBJ := $(23_pubsubmix_OUTPUT_OBJ)/generated

23_generated_SRC  :=
23_generated_OBJS :=

include $(23_pubsubmix_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(23_generated_SRC), $(eval $(call obj, $(cpp), $(23_generated_OUTPUT_OBJ), 23_generated_OBJS, $(23_pubsubmix_CXXFLAGS))))

DEPS = $(23_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(23_generated_TARGET_BIN): $(23_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
