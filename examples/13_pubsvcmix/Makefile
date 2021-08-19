13_pubsvcmix_BASE       := $(AREG_EXAMPLES)/13_pubsvcmix
13_pubsvcmix_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/13_pubsvcmix

13_pubsvcmix_CXXFLAGS   = -I$(13_pubsvcmix_BASE) $(examples_CXXFLAGS)
13_pubsvcmix_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(13_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(13_pubsvcmix_BASE)/generated/Makefile
include $(13_pubsvcmix_BASE)/ipcmixclient/Makefile
include $(13_pubsvcmix_BASE)/ipcmixmain/Makefile
include $(13_pubsvcmix_BASE)/ipcmixsecond/Makefile

13_pubsvcmix: $(AREG_OUTPUT_BIN)/$(13_ipcmixclient_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(13_ipcmixmain_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(13_ipcmixsecond_TARGET_BIN)

.PHONY: 13_pubsvcmix
