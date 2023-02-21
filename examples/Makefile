ifeq ($(AREG_BINARY), shared)
    examples_CXXFLAGS += -DIMP_AREG_DLL
else
    examples_CXXFLAGS += -DIMP_AREG_LIB
endif
examples_LDFLAGS  += $(AREG_LIB_INCLUDES) -lareg $(LDFLAGS)

include $(AREG_EXAMPLES)/00_helloservice/Makefile
include $(AREG_EXAMPLES)/01_hello/Makefile
include $(AREG_EXAMPLES)/02_buffer/Makefile
include $(AREG_EXAMPLES)/03_file/Makefile
include $(AREG_EXAMPLES)/04_trace/Makefile
include $(AREG_EXAMPLES)/05_timer/Makefile
include $(AREG_EXAMPLES)/06_threads/Makefile
include $(AREG_EXAMPLES)/07_synch/Makefile
include $(AREG_EXAMPLES)/08_service/Makefile
include $(AREG_EXAMPLES)/09_svcmulti/Makefile
include $(AREG_EXAMPLES)/10_locsvc/Makefile
include $(AREG_EXAMPLES)/11_locmesh/Makefile
include $(AREG_EXAMPLES)/12_pubsvc/Makefile
include $(AREG_EXAMPLES)/13_pubmesh/Makefile
include $(AREG_EXAMPLES)/14_pubtraffic/Makefile
include $(AREG_EXAMPLES)/15_pubworker/Makefile
include $(AREG_EXAMPLES)/16_pubfsm/Makefile
include $(AREG_EXAMPLES)/18_locwatchdog/Makefile
include $(AREG_EXAMPLES)/19_pubwatchdog/Makefile
include $(AREG_EXAMPLES)/20_pubdatarate/Makefile
include $(AREG_EXAMPLES)/21_pubunblock/Makefile

examples:               \
    00_helloservice     \
    01_hello            \
    02_buffer           \
    03_file             \
    04_trace            \
    05_timer            \
    06_threads          \
    07_synch            \
    08_service          \
    09_svcmulti         \
    10_locsvc           \
    11_locmesh          \
    12_pubsvc           \
    13_pubmesh          \
    14_pubtraffic       \
    15_pubworker        \
    16_pubfsm           \
    18_locwatchdog      \
    19_pubwatchdog      \
    20_pubdatarate      \
    21_pubunblock

.PHONY: examples
