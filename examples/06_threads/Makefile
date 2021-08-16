06_threads_PROJECT_NAME := 06_threads

06_threads_BASE := $(AREG_EXAMPLES)/06_threads

06_threads_TARGET_BIN := $(06_threads_PROJECT_NAME)$(AREG_BIN_EXT)
06_threads_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(06_threads_PROJECT_NAME)

06_threads_SRC  :=
06_threads_OBJS :=

# build list of source files used by 06_threads
include $(06_threads_BASE)/src/Makefile

06_threads: $(AREG_OUTPUT_BIN)/$(06_threads_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(06_threads_SRC), $(eval $(call obj, $(cpp), $(06_threads_OUTPUT_OBJ), 06_threads_OBJS, $(examples_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(06_threads_TARGET_BIN): $(areg_TARGET_PATH) $(06_threads_OBJS)
	@echo "Linking $(06_threads_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(06_threads_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 06_threads
