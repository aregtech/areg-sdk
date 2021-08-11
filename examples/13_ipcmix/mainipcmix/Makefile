13_mainipcmix_PROJECT_NAME := 13_mainipcmix

13_mainipcmix_BASE := $(ipcmix_BASE)/mainipcmix

13_mainipcmix_TARGET_BIN := $(13_mainipcmix_PROJECT_NAME)$(AREG_BIN_EXT)
13_mainipcmix_OUTPUT_OBJ := $(ipcmix_OUTPUT_OBJ)/$(13_mainipcmix_PROJECT_NAME)

13_mainipcmix_SRC  :=
13_mainipcmix_OBJS :=

include $(13_mainipcmix_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_mainipcmix_SRC), $(eval $(call obj, $(cpp), $(13_mainipcmix_OUTPUT_OBJ), 13_mainipcmix_OBJS, $(ipcmix_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(13_mainipcmix_TARGET_BIN): $(areg_OBJS) $(13_shareipcmix_OBJS) $(13_mainipcmix_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $^ $(LDFLAGS) -o $@
