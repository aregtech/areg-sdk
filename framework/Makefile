areg_PROJECT_NAME           := $(AREG_PROJECT)
areg_TARGET_BIN	            :=

extend_PROJECT_NAME         := $(AREG_EXTENDED_PROJECT)
extend_TARGET_BIN           := $(AREG_OUTPUT_LIB)/$(AREG_EXTEND_LIB)

logobserverapi_PROJECT_NAME := logobserverapi
logobserverapi_TARGET_BIN   := 

mcrouter_PROJECT_NAME       := mcrouter
mcrouter_TARGET_BIN         := $(AREG_OUTPUT_BIN)/$(mcrouter_PROJECT_NAME)$(AREG_BIN_EXT)

logger_PROJECT_NAME         := logger
logger_TARGET_BIN           := $(AREG_OUTPUT_BIN)/$(logger_PROJECT_NAME)$(AREG_BIN_EXT)

logobserver_PROJECT_NAME    := logobserver
logobserver_TARGET_BIN	    := $(AREG_OUTPUT_BIN)/$(logobserver_PROJECT_NAME)$(AREG_BIN_EXT)


include $(AREG_BASE)/areg/Makefile
include $(AREG_BASE)/extend/Makefile
include $(AREG_BASE)/mcrouter/Makefile
include $(AREG_BASE)/logger/Makefile
include $(AREG_BASE)/logobserver/Makefile


framework: areg extend mcrouter logger observer

.PHONY: framework
