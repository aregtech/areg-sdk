14_ipctraffic_PROJECT_NAME := 14_ipctraffic

14_ipctraffic_BASE := $(14_pubsvclight_BASE)/ipctraffic

14_ipctraffic_TARGET_BIN := $(14_ipctraffic_PROJECT_NAME)$(AREG_BIN_EXT)
14_ipctraffic_OUTPUT_OBJ := $(14_pubsvclight_OUTPUT_OBJ)/$(14_ipctraffic_PROJECT_NAME)

14_ipctraffic_SRC  :=
14_ipctraffic_OBJS :=

include $(14_ipctraffic_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(14_ipctraffic_SRC), $(eval $(call obj, $(cpp), $(14_ipctraffic_OUTPUT_OBJ), 14_ipctraffic_OBJS, $(14_pubsvclight_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(14_ipctraffic_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(14_generated_TARGET_BIN) $(14_ipctraffic_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(14_pubsvclight_CXXFLAGS) $(14_ipctraffic_OBJS) $(14_pubsvclight_LDFLAGS) -o $@
