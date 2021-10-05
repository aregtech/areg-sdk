14_generated_PROJECT_NAME := 14_generated

14_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(14_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
14_generated_OUTPUT_OBJ := $(14_pubtraffic_OUTPUT_OBJ)/$(14_generated_PROJECT_NAME)

14_generated_SRC  :=
14_generated_OBJS :=

include $(14_pubtraffic_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(14_generated_SRC), $(eval $(call obj, $(cpp), $(14_generated_OUTPUT_OBJ), 14_generated_OBJS, $(14_pubtraffic_CXXFLAGS))))

DEPS = $(14_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(14_generated_TARGET_BIN): $(14_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
