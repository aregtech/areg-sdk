22_generated_PROJECT_NAME := 22_generated

22_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(22_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
22_generated_OUTPUT_OBJ := $(22_pubsub_OUTPUT_OBJ)/generated

22_generated_SRC  :=
22_generated_OBJS :=

include $(22_pubsub_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(22_generated_SRC), $(eval $(call obj, $(cpp), $(22_generated_OUTPUT_OBJ), 22_generated_OBJS, $(22_pubsub_CXXFLAGS))))

DEPS = $(22_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(22_generated_TARGET_BIN): $(22_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
