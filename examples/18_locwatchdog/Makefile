18_locwatchdog_BASE       := $(AREG_EXAMPLES)/18_locwatchdog
18_locwatchdog_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/18_locwatchdog

18_locwatchdog_CXXFLAGS   = -I$(18_locwatchdog_BASE) $(examples_CXXFLAGS)
18_locwatchdog_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(18_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(18_locwatchdog_BASE)/generated/Makefile
include $(18_locwatchdog_BASE)/locservice/Makefile

18_locwatchdog: $(AREG_OUTPUT_BIN)/$(18_locservice_TARGET_BIN)

.PHONY: 18_locwatchdog
