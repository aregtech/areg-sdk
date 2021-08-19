15_pubsvcworker_BASE       := $(AREG_EXAMPLES)/15_pubsvcworker
15_pubsvcworker_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/15_pubsvcworker

15_pubsvcworker_CXXFLAGS   = -I$(15_pubsvcworker_BASE) $(examples_CXXFLAGS)
15_pubsvcworker_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(15_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(15_pubsvcworker_BASE)/generated/Makefile
include $(15_pubsvcworker_BASE)/ipchwmgr/Makefile
include $(15_pubsvcworker_BASE)/ipcsvcpatient/Makefile

15_pubsvcworker: $(AREG_OUTPUT_BIN)/$(15_ipchwmgr_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(15_ipcsvcpatient_TARGET_BIN)

.PHONY: 15_pubsvcworker
