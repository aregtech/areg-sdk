00_serviceproc_OUTPUT_OBJ := $(00_helloservice_OUTPUT_OBJ)/serviceproc

00_serviceproc_SRC  :=
00_serviceproc_OBJS :=

include $(00_helloservice_BASE)/multiprocess/serviceproc/src/Makefile

# define one target for each source file
$(foreach cpp, $(00_serviceproc_SRC), $(eval $(call obj, $(cpp), $(00_serviceproc_OUTPUT_OBJ), 00_serviceproc_OBJS, $(00_helloservice_CXXFLAGS))))

DEPS = $(00_serviceproc_OBJS:%.o=%.d)
-include $(DEPS)

$(00_serviceproc_TARGET_BIN): $(areg_TARGET_BIN) $(00_serviceproc_OBJS) $(00_helloservice_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(00_helloservice_CXXFLAGS) $(00_serviceproc_OBJS) $(00_helloservice_LDFLAGS) -o $@
