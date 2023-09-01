20_pubservice_OUTPUT_OBJ := $(20_pubdatarate_OUTPUT_OBJ)/pubservice

20_pubservice_SRC  :=
20_pubservice_OBJS :=

include $(20_pubdatarate_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(20_pubservice_SRC), $(eval $(call obj, $(cpp), $(20_pubservice_OUTPUT_OBJ), 20_pubservice_OBJS, $(20_pubdatarate_CXXFLAGS))))

DEPS = $(20_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(20_pubservice_TARGET_BIN): $(areg_TARGET_PATH) $(20_pubservice_OBJS) $(20_pubdatarate_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(20_pubdatarate_CXXFLAGS) $(20_pubservice_OBJS) -o $@ $(20_pubdatarate_LDFLAGS)
