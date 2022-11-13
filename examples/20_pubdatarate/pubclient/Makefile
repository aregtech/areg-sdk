20_pubclientdatarate_PROJECT_NAME := 20_pubclientdatarate

20_pubclientdatarate_TARGET_BIN := $(20_pubclientdatarate_PROJECT_NAME)$(AREG_BIN_EXT)
20_pubclientdatarate_OUTPUT_OBJ := $(20_pubdatarate_OUTPUT_OBJ)/pubclient

20_pubclientdatarate_SRC  :=
20_pubclientdatarate_OBJS :=

include $(20_pubdatarate_BASE)/pubclient/src/Makefile

# define one target for each source file
$(foreach cpp, $(20_pubclientdatarate_SRC), $(eval $(call obj, $(cpp), $(20_pubclientdatarate_OUTPUT_OBJ), 20_pubclientdatarate_OBJS, $(20_pubdatarate_CXXFLAGS))))

DEPS = $(20_pubclientdatarate_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(20_pubclientdatarate_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(20_generated_TARGET_BIN) $(20_clientlargedata_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(20_pubdatarate_CXXFLAGS) $(20_pubclientdatarate_OBJS) $(20_pubdatarate_LDFLAGS) -o $@
