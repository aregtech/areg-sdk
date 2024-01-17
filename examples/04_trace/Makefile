04_trace_PROJECT_NAME := 04_trace
04_trace_BASE		:= $(AREG_EXAMPLES)/04_trace
04_trace_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(04_trace_PROJECT_NAME)$(AREG_BIN_EXT)
04_trace_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(04_trace_PROJECT_NAME)

04_trace_SRC  :=
04_trace_OBJS :=

# build list of source files used by 04_trace
include $(04_trace_BASE)/src/Makefile

04_trace: $(04_trace_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(04_trace_SRC), $(eval $(call obj, $(cpp), $(04_trace_OUTPUT_OBJ), 04_trace_OBJS, $(examples_CXXFLAGS))))

$(04_trace_TARGET_BIN): $(areg_TARGET_BIN) $(04_trace_OBJS) $(AREG_DEPEND)
	@echo "Linking $(04_trace_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(04_trace_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 04_trace
