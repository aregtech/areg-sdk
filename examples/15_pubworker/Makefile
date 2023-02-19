15_pubworker_BASE       := $(AREG_EXAMPLES)/15_pubworker
15_pubworker_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/15_pubworker

15_pubworker_CXXFLAGS   = -I$(15_pubworker_BASE) $(examples_CXXFLAGS)
15_pubworker_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(15_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(15_pubworker_BASE)/generated/Makefile
include $(15_pubworker_BASE)/pubclient/Makefile
include $(15_pubworker_BASE)/pubservice/Makefile

15_pubworker: $(AREG_OUTPUT_BIN)/$(15_pubclient_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(15_pubservice_TARGET_BIN)

.PHONY: 15_pubworker
