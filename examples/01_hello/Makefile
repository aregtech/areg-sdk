01_hello_PROJECT_NAME := 01_hello

01_hello_BASE := $(AREG_EXAMPLES)/01_hello

01_hello_TARGET_BIN := $(01_hello_PROJECT_NAME)$(AREG_BIN_EXT)
01_hello_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(01_hello_PROJECT_NAME)

01_hello_SRC  :=
01_hello_OBJS :=

# build list of source files used by 01_hello
include $(01_hello_BASE)/src/Makefile

01_hello: $(AREG_OUTPUT_BIN)/$(01_hello_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(01_hello_SRC), $(eval $(call obj, $(cpp), $(01_hello_OUTPUT_OBJ), 01_hello_OBJS, $(examples_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(01_hello_TARGET_BIN): $(areg_TARGET_PATH) $(01_hello_OBJS) $(AREG_DEPEND)
	@echo "Linking $(01_hello_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(01_hello_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 01_hello
