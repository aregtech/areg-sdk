00_helloservice_BASE       := $(AREG_EXAMPLES)/00_helloservice
00_helloservice_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/00_helloservice

# Name of projects and location of binaries
00_generated_PROJECT_NAME	:= 00_generated
00_generated_TARGET_BIN		:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(00_generated_PROJECT_NAME)$(AREG_STATIC_LIB)

00_clientproc_PROJECT_NAME	:= 00_pubclient
00_clientproc_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(00_clientproc_PROJECT_NAME)$(AREG_BIN_EXT)

00_serviceproc_PROJECT_NAME := 00_pubservice
00_serviceproc_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(00_serviceproc_PROJECT_NAME)$(AREG_BIN_EXT)

00_onethread_PROJECT_NAME	:= 00_onethread
00_onethread_TARGET_BIN		:= $(AREG_OUTPUT_BIN)/$(00_onethread_PROJECT_NAME)$(AREG_BIN_EXT)

00_twothreads_PROJECT_NAME	:= 00_twothreads
00_twothreads_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(00_twothreads_PROJECT_NAME)$(AREG_BIN_EXT)

# Compiler and linker flags
00_helloservice_CXXFLAGS	= -I$(00_helloservice_BASE) $(examples_CXXFLAGS)
00_helloservice_LDFLAGS		= $(AREG_LIB_INCLUDES) -lareg -l$(00_generated_PROJECT_NAME) $(LDFLAGS)

# Set build dependencies
00_helloservice_DEPEND		:= $(AREG_DEPEND) $(00_generated_TARGET_BIN)

# 'generated' have to be first
include $(00_helloservice_BASE)/generated/Makefile
include $(00_helloservice_BASE)/onethread/Makefile
include $(00_helloservice_BASE)/twothreads/Makefile
include $(00_helloservice_BASE)/multiprocess/serviceproc/Makefile
include $(00_helloservice_BASE)/multiprocess/clientproc/Makefile

00_helloservice: $(00_generated_TARGET_BIN) $(00_onethread_TARGET_BIN) $(00_twothreads_TARGET_BIN) $(00_serviceproc_TARGET_BIN) $(00_clientproc_TARGET_BIN)

.PHONY: 00_helloservice
