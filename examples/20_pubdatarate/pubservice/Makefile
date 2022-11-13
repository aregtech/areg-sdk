20_pubsvcdatarate_PROJECT_NAME := 20_pubdatarate

20_pubsvcdatarate_TARGET_BIN := $(20_pubsvcdatarate_PROJECT_NAME)$(AREG_BIN_EXT)
20_pubsvcdatarate_OUTPUT_OBJ := $(20_pubdatarate_OUTPUT_OBJ)/pubservice

20_pubsvcdatarate_SRC  :=
20_pubsvcdatarate_OBJS :=

include $(20_pubdatarate_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(20_pubsvcdatarate_SRC), $(eval $(call obj, $(cpp), $(20_pubsvcdatarate_OUTPUT_OBJ), 20_pubsvcdatarate_OBJS, $(20_pubdatarate_CXXFLAGS))))

DEPS = $(20_pubsvcdatarate_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(20_pubsvcdatarate_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(20_generated_TARGET_BIN) $(20_pubsvcdatarate_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(20_pubdatarate_CXXFLAGS) $(20_pubsvcdatarate_OBJS) $(20_pubdatarate_LDFLAGS) -o $@
