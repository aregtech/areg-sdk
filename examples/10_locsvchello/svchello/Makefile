10_svchello_PROJECT_NAME := 10_svchello

10_svchello_BASE := $(10_locsvchello_BASE)/svchello

10_svchello_TARGET_BIN := $(10_svchello_PROJECT_NAME)$(AREG_BIN_EXT)
10_svchello_OUTPUT_OBJ := $(10_locsvchello_OUTPUT_OBJ)/$(10_svchello_PROJECT_NAME)

10_svchello_SRC  :=
10_svchello_OBJS :=

include $(10_svchello_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(10_svchello_SRC), $(eval $(call obj, $(cpp), $(10_svchello_OUTPUT_OBJ), 10_svchello_OBJS, $(10_locsvchello_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(10_svchello_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(10_generated_TARGET_BIN) $(10_svchello_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(10_locsvchello_CXXFLAGS) $(10_svchello_OBJS) $(10_locsvchello_LDFLAGS) -o $@
