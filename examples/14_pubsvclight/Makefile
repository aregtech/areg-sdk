14_pubsvclight_BASE       := $(AREG_EXAMPLES)/14_pubsvclight
14_pubsvclight_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/14_pubsvclight

14_pubsvclight_CXXFLAGS   = -I$(14_pubsvclight_BASE) $(examples_CXXFLAGS)
14_pubsvclight_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(14_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(14_pubsvclight_BASE)/generated/Makefile
include $(14_pubsvclight_BASE)/ipctraffic/Makefile
include $(14_pubsvclight_BASE)/ipclightctrl/Makefile

14_pubsvclight: $(AREG_OUTPUT_BIN)/$(14_ipctraffic_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(14_ipclightctrl_TARGET_BIN)

.PHONY: 14_pubsvclight
