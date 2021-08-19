13_ipcmixmain_PROJECT_NAME := 13_ipcmixmain

13_ipcmixmain_BASE := $(13_pubsvcmix_BASE)/ipcmixmain

13_ipcmixmain_TARGET_BIN := $(13_ipcmixmain_PROJECT_NAME)$(AREG_BIN_EXT)
13_ipcmixmain_OUTPUT_OBJ := $(13_pubsvcmix_OUTPUT_OBJ)/$(13_ipcmixmain_PROJECT_NAME)

13_ipcmixmain_SRC  :=
13_ipcmixmain_OBJS :=

include $(13_ipcmixmain_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_ipcmixmain_SRC), $(eval $(call obj, $(cpp), $(13_ipcmixmain_OUTPUT_OBJ), 13_ipcmixmain_OBJS, $(13_pubsvcmix_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(13_ipcmixmain_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(13_generated_TARGET_BIN) $(13_ipcmixmain_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(13_pubsvcmix_CXXFLAGS) $(13_ipcmixmain_OBJS) $(13_pubsvcmix_LDFLAGS) -o $@
