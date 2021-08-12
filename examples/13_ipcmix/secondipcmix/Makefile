13_secondipcmix_PROJECT_NAME := 13_secondipcmix

13_secondipcmix_BASE := $(ipcmix_BASE)/secondipcmix

13_secondipcmix_TARGET_BIN := $(13_secondipcmix_PROJECT_NAME)$(AREG_BIN_EXT)
13_secondipcmix_OUTPUT_OBJ := $(ipcmix_OUTPUT_OBJ)/$(13_secondipcmix_PROJECT_NAME)

13_secondipcmix_SRC  :=
13_secondipcmix_OBJS :=

include $(13_secondipcmix_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_secondipcmix_SRC), $(eval $(call obj, $(cpp), $(13_secondipcmix_OUTPUT_OBJ), 13_secondipcmix_OBJS, $(ipcmix_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(13_secondipcmix_TARGET_BIN): $(areg_OBJS) $(13_shareipcmix_OBJS) $(13_secondipcmix_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(ipcmix_CXXFLAGS) $^ $(LDFLAGS) -o $@
