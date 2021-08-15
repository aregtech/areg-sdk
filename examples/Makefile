ifeq ($(AREG_BINARY), shared)
	examples_CXXFLAGS += -DIMP_AREG_DLL
	examples_LDFLAGS  += -Wl,-rpath=$(AREG_OUTPUT_BIN) -L $(AREG_OUTPUT_BIN) -lareg $(LDFLAGS)
else
	examples_CXXFLAGS += -DIMP_AREG_LIB
	examples_LDFLAGS  += -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -lareg -Wl,-Bdynamic $(LDFLAGS)
endif

include $(AREG_EXAMPLES)/01_hello/Makefile
include $(AREG_EXAMPLES)/13_pubsvcmix/Makefile

examples: 01_hello 13_pubsvcmix

.PHONY: examples
