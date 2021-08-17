12_generated_PROJECT_NAME := 12_generated

12_generated_BASE := $(12_pubsvchello_BASE)/generated

12_generated_TARGET_BIN := $(AREG_LIB_PREFIX)$(12_generated_PROJECT_NAME)$(AREG_STATIC_LIB)
12_generated_OUTPUT_OBJ := $(12_pubsvchello_OUTPUT_OBJ)/$(12_generated_PROJECT_NAME)

12_generated_SRC  :=
12_generated_OBJS :=

include $(12_generated_BASE)/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(12_generated_SRC), $(eval $(call obj, $(cpp), $(12_generated_OUTPUT_OBJ), 12_generated_OBJS, $(12_pubsvchello_CXXFLAGS))))

$(AREG_OUTPUT_LIB)/$(12_generated_TARGET_BIN): $(12_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
