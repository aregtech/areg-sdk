00_onethread_PROJECT_NAME := 00_onethread

00_onethread_TARGET_BIN := $(00_onethread_PROJECT_NAME)$(AREG_BIN_EXT)
00_onethread_OUTPUT_OBJ := $(00_helloworld_OUTPUT_OBJ)/$(00_onethread_PROJECT_NAME)

00_onethread_SRC  :=
00_onethread_OBJS :=

include $(00_helloworld_BASE)/onethread/src/Makefile

# define one target for each source file
$(foreach cpp, $(00_onethread_SRC), $(eval $(call obj, $(cpp), $(00_onethread_OUTPUT_OBJ), 00_onethread_OBJS, $(00_helloworld_CXXFLAGS))))

DEPS = $(00_onethread_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(00_onethread_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN) $(00_onethread_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(00_helloworld_CXXFLAGS) $(00_onethread_OBJS) $(00_helloworld_LDFLAGS) -o $@
