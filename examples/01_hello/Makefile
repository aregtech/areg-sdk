hello_PROJECT_NAME := 01_hello

hello_BASE := $(AREG_EXAMPLES)/01_hello

hello_TARGET_BIN  :=

hello_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(hello_PROJECT_NAME)
hello_OUTPUT_LIB := $(AREG_OUTPUT_LIB)
hello_OUTPUT_BIN := $(AREG_OUTPUT_BIN)

hello_SRC  :=
hello_OBJS :=

# build list of source files used by 01_hello
include $(hello_BASE)/src/Makefile

01_hello_CXXFLAGS := -DIMP_AREG_LIB

hello_TARGET_BIN := $(hello_PROJECT_NAME)$(AREG_BIN_EXT)

01_hello: $(hello_OUTPUT_BIN)/$(hello_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(hello_SRC), $(eval $(call obj, $(cpp), $(hello_OUTPUT_OBJ), hello_OBJS, $(01_hello_CXXFLAGS))))

$(hello_OUTPUT_BIN)/$(hello_TARGET_BIN): $(areg_OBJS) $(hello_OBJS)
	@echo "Linking $(hello_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $^ $(LDFLAGS) -o $@

clean_hello:
	rm -f $(hello_OUTPUT_BIN)/$(hello_TARGET_BIN)

.PHONY: 01_hello clean_hello
