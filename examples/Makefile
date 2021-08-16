ifeq ($(AREG_BINARY), shared)
	examples_CXXFLAGS += -DIMP_AREG_DLL
	examples_LDFLAGS  += -Wl,-rpath=$(AREG_OUTPUT_BIN) -L $(AREG_OUTPUT_BIN) -lareg $(LDFLAGS)
else
	examples_CXXFLAGS += -DIMP_AREG_LIB
	examples_LDFLAGS  += -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -lareg -Wl,-Bdynamic $(LDFLAGS)
endif

include $(AREG_EXAMPLES)/01_hello/Makefile
include $(AREG_EXAMPLES)/02_buffer/Makefile
include $(AREG_EXAMPLES)/03_file/Makefile
include $(AREG_EXAMPLES)/04_trace/Makefile
include $(AREG_EXAMPLES)/05_timer/Makefile
include $(AREG_EXAMPLES)/06_threads/Makefile
include $(AREG_EXAMPLES)/07_synch/Makefile
include $(AREG_EXAMPLES)/08_service/Makefile
include $(AREG_EXAMPLES)/09_svcmulti/Makefile
include $(AREG_EXAMPLES)/10_locsvchello/Makefile
include $(AREG_EXAMPLES)/13_pubsvcmix/Makefile

examples: 01_hello 02_buffer 03_file 04_trace 05_timer \
	06_threads 07_synch 08_service 09_svcmulti \
	10_locsvchello 13_pubsvcmix

.PHONY: examples
