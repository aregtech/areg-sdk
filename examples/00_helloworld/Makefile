00_helloworld_BASE       := $(AREG_EXAMPLES)/00_helloworld
00_helloworld_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/00_helloworld

00_helloworld_CXXFLAGS   = -I$(00_helloworld_BASE) $(examples_CXXFLAGS)
00_helloworld_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(00_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(00_helloworld_BASE)/generated/Makefile
include $(00_helloworld_BASE)/onethread/Makefile
include $(00_helloworld_BASE)/twothreads/Makefile
include $(00_helloworld_BASE)/multiprocess/serviceproc/Makefile
include $(00_helloworld_BASE)/multiprocess/clientproc/Makefile

00_helloworld: $(AREG_OUTPUT_BIN)/$(00_onethread_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(00_twothreads_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(00_serviceproc_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(00_clientproc_TARGET_BIN)

.PHONY: 00_helloworld
