# example project 13_pubsvcmix -> 13_ipcmixsecond

13_ipcmixsecond_PROJECT_NAME := 13_ipcmixsecond

13_ipcmixsecond_BASE := $(13_pubsvcmix_BASE)/ipcmixsecond

13_ipcmixsecond_TARGET_BIN := $(13_ipcmixsecond_PROJECT_NAME)$(AREG_BIN_EXT)
13_ipcmixsecond_OUTPUT_OBJ := $(13_pubsvcmix_OUTPUT_OBJ)/$(13_ipcmixsecond_PROJECT_NAME)

13_ipcmixsecond_SRC  :=
13_ipcmixsecond_OBJS :=

include $(13_ipcmixsecond_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_ipcmixsecond_SRC), $(eval $(call obj, $(cpp), $(13_ipcmixsecond_OUTPUT_OBJ), 13_ipcmixsecond_OBJS, $(13_pubsvcmix_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(13_ipcmixsecond_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(13_generated_TARGET_BIN) $(13_ipcmixsecond_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(13_pubsvcmix_CXXFLAGS) $(13_ipcmixsecond_OBJS) $(13_pubsvcmix_LDFLAGS) -o $@
