19_pubwatchdog_BASE       := $(AREG_EXAMPLES)/19_pubwatchdog
19_pubwatchdog_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/19_pubwatchdog

19_pubwatchdog_CXXFLAGS   = -I$(19_pubwatchdog_BASE) $(examples_CXXFLAGS)
19_pubwatchdog_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(19_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(19_pubwatchdog_BASE)/generated/Makefile
include $(19_pubwatchdog_BASE)/pubservice/Makefile
include $(19_pubwatchdog_BASE)/pubclient/Makefile

19_pubwatchdog: $(AREG_OUTPUT_BIN)/$(19_pubclient_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(19_pubservice_TARGET_BIN)

.PHONY: 19_pubwatchdog
