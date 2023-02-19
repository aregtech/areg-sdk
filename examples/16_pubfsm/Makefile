16_pubfsm_BASE       := $(AREG_EXAMPLES)/16_pubfsm
16_pubfsm_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/16_pubfsm

16_pubfsm_CXXFLAGS   = -I$(16_pubfsm_BASE) $(examples_CXXFLAGS)
16_pubfsm_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(16_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(16_pubfsm_BASE)/generated/Makefile
include $(16_pubfsm_BASE)/pubservice/Makefile
include $(16_pubfsm_BASE)/pubclient/Makefile

16_pubfsm: $(AREG_OUTPUT_BIN)/$(16_pubservice_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(16_pubclient_TARGET_BIN)

.PHONY: 16_pubfsm
