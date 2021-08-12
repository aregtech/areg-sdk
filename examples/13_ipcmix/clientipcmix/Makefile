13_clientipcmix_PROJECT_NAME := 13_clientipcmix

13_clientipcmix_BASE := $(ipcmix_BASE)/clientipcmix

13_clientipcmix_TARGET_BIN := $(13_clientipcmix_PROJECT_NAME)$(AREG_BIN_EXT)
13_clientipcmix_OUTPUT_OBJ := $(ipcmix_OUTPUT_OBJ)/$(13_clientipcmix_PROJECT_NAME)

13_clientipcmix_SRC  :=
13_clientipcmix_OBJS :=

include $(13_clientipcmix_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_clientipcmix_SRC), $(eval $(call obj, $(cpp), $(13_clientipcmix_OUTPUT_OBJ), 13_clientipcmix_OBJS, $(ipcmix_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(13_clientipcmix_TARGET_BIN): $(areg_OBJS) $(13_shareipcmix_OBJS) $(13_clientipcmix_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(ipcmix_CXXFLAGS) $^ $(LDFLAGS) -o $@
