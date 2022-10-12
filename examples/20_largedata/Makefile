20_largedata_BASE       := $(AREG_EXAMPLES)/20_largedata
20_largedata_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/20_largedata

20_largedata_CXXFLAGS   = -I$(20_largedata_BASE) $(examples_CXXFLAGS)
20_largedata_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(20_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(20_largedata_BASE)/generated/Makefile
include $(20_largedata_BASE)/pubservice/Makefile
include $(20_largedata_BASE)/pubclient/Makefile

20_largedata: $(AREG_OUTPUT_BIN)/$(20_clientlargedata_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(20_svclargedata_TARGET_BIN)

.PHONY: 20_largedata
