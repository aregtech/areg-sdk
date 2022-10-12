20_svclargedata_PROJECT_NAME := 20_largedata

20_svclargedata_TARGET_BIN := $(20_svclargedata_PROJECT_NAME)$(AREG_BIN_EXT)
20_svclargedata_OUTPUT_OBJ := $(20_largedata_OUTPUT_OBJ)/pubservice

20_svclargedata_SRC  :=
20_svclargedata_OBJS :=

include $(20_largedata_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(20_svclargedata_SRC), $(eval $(call obj, $(cpp), $(20_svclargedata_OUTPUT_OBJ), 20_svclargedata_OBJS, $(20_largedata_CXXFLAGS))))

DEPS = $(20_svclargedata_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(20_svclargedata_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(20_generated_TARGET_BIN) $(20_svclargedata_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(20_largedata_CXXFLAGS) $(20_svclargedata_OBJS) $(20_largedata_LDFLAGS) -o $@
