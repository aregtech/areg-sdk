00_onethread_OUTPUT_OBJ := $(00_helloservice_OUTPUT_OBJ)/onethread

00_onethread_SRC  :=
00_onethread_OBJS :=

include $(00_helloservice_BASE)/onethread/src/Makefile

# define one target for each source file
$(foreach cpp, $(00_onethread_SRC), $(eval $(call obj, $(cpp), $(00_onethread_OUTPUT_OBJ), 00_onethread_OBJS, $(00_helloservice_CXXFLAGS))))

DEPS = $(00_onethread_OBJS:%.o=%.d)
-include $(DEPS)

$(00_onethread_TARGET_BIN): $(areg_TARGET_BIN) $(00_onethread_OBJS) $(00_helloservice_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(00_helloservice_CXXFLAGS) $(00_onethread_OBJS) -o $@ $(00_helloservice_LDFLAGS) 
