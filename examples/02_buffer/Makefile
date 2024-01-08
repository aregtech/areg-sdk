02_buffer_PROJECT_NAME	:= 02_buffer
02_buffer_BASE			:= $(AREG_EXAMPLES)/02_buffer
02_buffer_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(02_buffer_PROJECT_NAME)$(AREG_BIN_EXT)
02_buffer_OUTPUT_OBJ	:= $(AREG_OUTPUT_OBJ)/$(02_buffer_PROJECT_NAME)

02_buffer_SRC  :=
02_buffer_OBJS :=

# build list of source files used by 02_buffer
include $(02_buffer_BASE)/src/Makefile

02_buffer: $(02_buffer_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(02_buffer_SRC), $(eval $(call obj, $(cpp), $(02_buffer_OUTPUT_OBJ), 02_buffer_OBJS, $(examples_CXXFLAGS))))

$(02_buffer_TARGET_BIN): $(areg_TARGET_BIN) $(02_buffer_OBJS) $(AREG_DEPEND)
	@echo "Linking $(02_buffer_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(02_buffer_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 02_buffer
