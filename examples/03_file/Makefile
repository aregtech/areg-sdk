03_file_PROJECT_NAME := 03_file

03_file_BASE := $(AREG_EXAMPLES)/03_file

03_file_TARGET_BIN := $(03_file_PROJECT_NAME)$(AREG_BIN_EXT)
03_file_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(03_file_PROJECT_NAME)

03_file_SRC  :=
03_file_OBJS :=

# build list of source files used by 03_file
include $(03_file_BASE)/src/Makefile

03_file: $(AREG_OUTPUT_BIN)/$(03_file_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(03_file_SRC), $(eval $(call obj, $(cpp), $(03_file_OUTPUT_OBJ), 03_file_OBJS, $(examples_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(03_file_TARGET_BIN): $(areg_TARGET_PATH) $(03_file_OBJS) $(AREG_DEPEND)
	@echo "Linking $(03_file_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(03_file_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 03_file
