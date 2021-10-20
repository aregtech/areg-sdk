00_seviceproc_PROJECT_NAME := 00_seviceproc

00_seviceproc_TARGET_BIN := $(00_seviceproc_PROJECT_NAME)$(AREG_BIN_EXT)
00_seviceproc_OUTPUT_OBJ := $(00_helloworld_OUTPUT_OBJ)/$(00_seviceproc_PROJECT_NAME)

00_seviceproc_SRC  :=
00_seviceproc_OBJS :=

include $(00_helloworld_BASE)/onethread/src/Makefile

# define one target for each source file
$(foreach cpp, $(00_seviceproc_SRC), $(eval $(call obj, $(cpp), $(00_seviceproc_OUTPUT_OBJ), 00_seviceproc_OBJS, $(00_helloworld_CXXFLAGS))))

DEPS = $(00_seviceproc_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(00_seviceproc_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN) $(00_seviceproc_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(00_helloworld_CXXFLAGS) $(00_seviceproc_OBJS) $(00_helloworld_LDFLAGS) -o $@
