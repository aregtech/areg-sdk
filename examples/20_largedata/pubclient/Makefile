20_clientlargedata_PROJECT_NAME := 20_clientlargedata

20_clientlargedata_TARGET_BIN := $(20_clientlargedata_PROJECT_NAME)$(AREG_BIN_EXT)
20_clientlargedata_OUTPUT_OBJ := $(20_largedata_OUTPUT_OBJ)/pubclient

20_clientlargedata_SRC  :=
20_clientlargedata_OBJS :=

include $(20_largedata_BASE)/pubclient/src/Makefile

# define one target for each source file
$(foreach cpp, $(20_clientlargedata_SRC), $(eval $(call obj, $(cpp), $(20_clientlargedata_OUTPUT_OBJ), 20_clientlargedata_OBJS, $(20_largedata_CXXFLAGS))))

DEPS = $(20_clientlargedata_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(20_clientlargedata_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(20_generated_TARGET_BIN) $(20_clientlargedata_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(20_largedata_CXXFLAGS) $(20_clientlargedata_OBJS) $(20_largedata_LDFLAGS) -o $@
