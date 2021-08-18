16_ipcfsmclient_PROJECT_NAME := 16_ipcfsmclient

16_ipcfsmclient_BASE := $(16_pubfsm_BASE)/ipcfsmclient

16_ipcfsmclient_TARGET_BIN := $(16_ipcfsmclient_PROJECT_NAME)$(AREG_BIN_EXT)
16_ipcfsmclient_OUTPUT_OBJ := $(16_pubfsm_OUTPUT_OBJ)/$(16_ipcfsmclient_PROJECT_NAME)

16_ipcfsmclient_SRC  :=
16_ipcfsmclient_OBJS :=

include $(16_ipcfsmclient_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(16_ipcfsmclient_SRC), $(eval $(call obj, $(cpp), $(16_ipcfsmclient_OUTPUT_OBJ), 16_ipcfsmclient_OBJS, $(16_pubfsm_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(16_ipcfsmclient_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(16_generated_TARGET_BIN) $(16_ipcfsmclient_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(16_pubfsm_CXXFLAGS) $(16_ipcfsmclient_OBJS) $(16_pubfsm_LDFLAGS) -o $@
