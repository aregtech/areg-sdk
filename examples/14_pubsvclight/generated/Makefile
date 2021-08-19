14_generated_PROJECT_NAME := 14_generated

14_generated_BASE := $(14_pubsvclight_BASE)/generated

14_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(14_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
14_generated_OUTPUT_OBJ := $(14_pubsvclight_OUTPUT_OBJ)/$(14_generated_PROJECT_NAME)

14_generated_SRC  :=
14_generated_OBJS :=

include $(14_generated_BASE)/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(14_generated_SRC), $(eval $(call obj, $(cpp), $(14_generated_OUTPUT_OBJ), 14_generated_OBJS, $(14_pubsvclight_CXXFLAGS))))

$(AREG_OUTPUT_LIB)/$(14_generated_TARGET_BIN): $(14_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
