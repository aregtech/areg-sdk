18_locwatchdog_BASE       := $(AREG_EXAMPLES)/18_locwatchdog
18_locwatchdog_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/18_locwatchdog

18_locwatchdog_CXXFLAGS   = -I$(18_locwatchdog_BASE) $(examples_CXXFLAGS)
18_locwatchdog_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(18_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(18_locwatchdog_BASE)/generated/Makefile
include $(18_locwatchdog_BASE)/locservice/Makefile

18_locwatchdog: $(AREG_OUTPUT_BIN)/$(18_locwatchdoge_TARGET_BIN)

.PHONY: 18_locwatchdog
