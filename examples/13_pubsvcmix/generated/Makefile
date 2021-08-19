13_generated_PROJECT_NAME := 13_generated

13_generated_BASE := $(13_pubsvcmix_BASE)/generated

13_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(13_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
13_generated_OUTPUT_OBJ := $(13_pubsvcmix_OUTPUT_OBJ)/$(13_generated_PROJECT_NAME)

13_generated_SRC  :=
13_generated_OBJS :=

include $(13_generated_BASE)/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(13_generated_SRC), $(eval $(call obj, $(cpp), $(13_generated_OUTPUT_OBJ), 13_generated_OBJS, $(13_pubsvcmix_CXXFLAGS))))

$(AREG_OUTPUT_LIB)/$(13_generated_TARGET_BIN): $(13_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
