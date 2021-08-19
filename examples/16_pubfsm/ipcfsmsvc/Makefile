16_ipcfsmsvc_PROJECT_NAME := 16_ipcfsmsvc

16_ipcfsmsvc_BASE := $(16_pubfsm_BASE)/ipcfsmsvc

16_ipcfsmsvc_TARGET_BIN := $(16_ipcfsmsvc_PROJECT_NAME)$(AREG_BIN_EXT)
16_ipcfsmsvc_OUTPUT_OBJ := $(16_pubfsm_OUTPUT_OBJ)/$(16_ipcfsmsvc_PROJECT_NAME)

16_ipcfsmsvc_SRC  :=
16_ipcfsmsvc_OBJS :=

include $(16_ipcfsmsvc_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(16_ipcfsmsvc_SRC), $(eval $(call obj, $(cpp), $(16_ipcfsmsvc_OUTPUT_OBJ), 16_ipcfsmsvc_OBJS, $(16_pubfsm_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(16_ipcfsmsvc_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(16_generated_TARGET_BIN) $(16_ipcfsmsvc_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(16_pubfsm_CXXFLAGS) $(16_ipcfsmsvc_OBJS) $(16_pubfsm_LDFLAGS) -o $@
