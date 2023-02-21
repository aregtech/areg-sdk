20_pubdatarate_BASE       := $(AREG_EXAMPLES)/20_pubdatarate
20_pubdatarate_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/20_pubdatarate

20_pubdatarate_CXXFLAGS   = -I$(20_pubdatarate_BASE) $(examples_CXXFLAGS)
20_pubdatarate_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(20_generated_PROJECT_NAME) -lareg-extensions -lareg $(LDFLAGS) $(AREG_EXTENSIONS_LIBS)

# 'generated' have to be first
include $(20_pubdatarate_BASE)/generated/Makefile
include $(20_pubdatarate_BASE)/pubservice/Makefile
include $(20_pubdatarate_BASE)/pubclient/Makefile

20_pubdatarate: $(AREG_OUTPUT_BIN)/$(20_pubclient_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(20_pubservice_TARGET_BIN)

.PHONY: 20_pubdatarate
