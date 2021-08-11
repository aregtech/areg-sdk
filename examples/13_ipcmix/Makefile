ipcmix_PROJECT_NAME := 13_ipcmix

ipcmix_BASE := $(AREG_EXAMPLES)/13_ipcmix

ipcmix_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(ipcmix_PROJECT_NAME)

ipcmix_CXXFLAGS := -I$(ipcmix_BASE) $(examples_CXXFLAGS)

# shareipcmix have to be first
include $(ipcmix_BASE)/shareipcmix/Makefile
include $(ipcmix_BASE)/mainipcmix/Makefile
include $(ipcmix_BASE)/clientipcmix/Makefile
include $(ipcmix_BASE)/secondipcmix/Makefile

13_ipcmix: $(AREG_OUTPUT_BIN)/$(13_mainipcmix_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(13_clientipcmix_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(13_secondipcmix_TARGET_BIN)

.PHONY: 13_ipcmix
