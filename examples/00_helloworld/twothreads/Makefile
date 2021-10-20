00_twothreads_PROJECT_NAME := 00_twothreads

00_twothreads_TARGET_BIN := $(00_twothreads_PROJECT_NAME)$(AREG_BIN_EXT)
00_twothreads_OUTPUT_OBJ := $(00_helloworld_OUTPUT_OBJ)/$(00_twothreads_PROJECT_NAME)

00_twothreads_SRC  :=
00_twothreads_OBJS :=

include $(00_helloworld_BASE)/twothreads/src/Makefile

# define one target for each source file
$(foreach cpp, $(00_twothreads_SRC), $(eval $(call obj, $(cpp), $(00_twothreads_OUTPUT_OBJ), 00_twothreads_OBJS, $(00_helloworld_CXXFLAGS))))

DEPS = $(00_twothreads_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(00_twothreads_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(00_generated_TARGET_BIN) $(00_twothreads_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(00_helloworld_CXXFLAGS) $(00_twothreads_OBJS) $(00_helloworld_LDFLAGS) -o $@
