00_clientproc_OUTPUT_OBJ := $(00_helloservice_OUTPUT_OBJ)/clientproc

00_clientproc_SRC  :=
00_clientproc_OBJS :=

include $(00_helloservice_BASE)/multiprocess/clientproc/src/Makefile

# define one target for each source file
$(foreach cpp, $(00_clientproc_SRC), $(eval $(call obj, $(cpp), $(00_clientproc_OUTPUT_OBJ), 00_clientproc_OBJS, $(00_helloservice_CXXFLAGS))))

DEPS = $(00_clientproc_OBJS:%.o=%.d)
-include $(DEPS)

$(00_clientproc_TARGET_BIN): $(areg_TARGET_BIN) $(00_clientproc_OBJS) $(00_helloservice_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(00_helloservice_CXXFLAGS) $(00_clientproc_OBJS) -o $@ $(00_helloservice_LDFLAGS)
