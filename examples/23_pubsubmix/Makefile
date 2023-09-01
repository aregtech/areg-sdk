23_pubsubmix_BASE       := $(AREG_EXAMPLES)/23_pubsubmix
23_pubsubmix_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/23_pubsubmix

23_pubsubmix_CXXFLAGS   = -I$(23_pubsubmix_BASE) $(examples_CXXFLAGS)
23_pubsubmix_LDFLAGS    = $(AREG_LIB_INCLUDES) -lareg -l$(23_common_PROJECT_NAME) -l$(23_generated_PROJECT_NAME) -l$(extend_PROJECT_NAME) $(LDFLAGS) $(AREG_EXTENDED_LIBS)

# 'generated' have to be first
include $(23_pubsubmix_BASE)/generated/Makefile
include $(23_pubsubmix_BASE)/common/Makefile
include $(23_pubsubmix_BASE)/pubsubctrl/Makefile
include $(23_pubsubmix_BASE)/pubsubdyn/Makefile

23_pubsubmix:	\
	$(AREG_OUTPUT_LIB)/$(23_generated_TARGET_BIN)	\
	$(AREG_OUTPUT_LIB)/$(23_common_TARGET_BIN)		\
	$(AREG_OUTPUT_BIN)/$(23_pubsubctrl_TARGET_BIN)	\
	$(AREG_OUTPUT_BIN)/$(23_pubsubdyn_TARGET_BIN)

.PHONY: 23_pubsubmix
