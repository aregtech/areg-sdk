12_pubsvc_BASE       := $(AREG_EXAMPLES)/12_pubsvc
12_pubsvc_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/12_pubsvc

12_pubsvc_CXXFLAGS   = -I$(12_pubsvc_BASE) $(examples_CXXFLAGS)
12_pubsvc_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(12_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(12_pubsvc_BASE)/generated/Makefile
include $(12_pubsvc_BASE)/pubservice/Makefile
include $(12_pubsvc_BASE)/pubclient/Makefile

12_pubsvc: $(AREG_OUTPUT_BIN)/$(12_pubservice_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(12_pubclient_TARGET_BIN)

.PHONY: 12_pubsvc
