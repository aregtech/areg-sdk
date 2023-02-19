00_helloservice_BASE       := $(AREG_EXAMPLES)/00_helloservice
00_helloservice_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/00_helloservice

00_helloservice_CXXFLAGS   = -I$(00_helloservice_BASE) $(examples_CXXFLAGS)
00_helloservice_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(00_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(00_helloservice_BASE)/generated/Makefile
include $(00_helloservice_BASE)/onethread/Makefile
include $(00_helloservice_BASE)/twothreads/Makefile
include $(00_helloservice_BASE)/multiprocess/serviceproc/Makefile
include $(00_helloservice_BASE)/multiprocess/clientproc/Makefile

00_helloservice: $(AREG_OUTPUT_BIN)/$(00_onethread_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(00_twothreads_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(00_serviceproc_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(00_clientproc_TARGET_BIN)

.PHONY: 00_helloservice
