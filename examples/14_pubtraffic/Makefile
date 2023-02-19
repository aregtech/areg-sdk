14_pubtraffic_BASE       := $(AREG_EXAMPLES)/14_pubtraffic
14_pubtraffic_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/14_pubtraffic

14_pubtraffic_CXXFLAGS   = -I$(14_pubtraffic_BASE) $(examples_CXXFLAGS)
14_pubtraffic_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(14_generated_PROJECT_NAME) -lareg-extensions -lareg $(LDFLAGS) $(AREG_EXTENSIONS_LIBS)

# 'generated' have to be first
include $(14_pubtraffic_BASE)/generated/Makefile
include $(14_pubtraffic_BASE)/pubclient/Makefile
include $(14_pubtraffic_BASE)/pubservice/Makefile

14_pubtraffic: $(AREG_OUTPUT_BIN)/$(14_pubclient_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(14_pubservice_TARGET_BIN)

.PHONY: 14_pubtraffic
