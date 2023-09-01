22_pubsub_BASE       := $(AREG_EXAMPLES)/22_pubsub
22_pubsub_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/22_pubsub

22_pubsub_CXXFLAGS   = -I$(22_pubsub_BASE) $(examples_CXXFLAGS)
22_pubsub_LDFLAGS    = $(AREG_LIB_INCLUDES) -lareg -l$(22_generated_PROJECT_NAME) -l$(extend_PROJECT_NAME) $(LDFLAGS) $(AREG_EXTENDED_LIBS)

# 'generated' have to be first
include $(22_pubsub_BASE)/generated/Makefile
include $(22_pubsub_BASE)/publisher/Makefile
include $(22_pubsub_BASE)/subscriber/Makefile

22_pubsub:	\
	$(AREG_OUTPUT_LIB)/$(22_generated_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(22_publisher_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(22_subscriber_TARGET_BIN)

.PHONY: 22_pubsub
