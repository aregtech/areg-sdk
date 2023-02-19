10_locsvc_BASE       := $(AREG_EXAMPLES)/10_locsvc
10_locsvc_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/10_locsvc

10_locsvc_CXXFLAGS   = -I$(10_locsvc_BASE) $(examples_CXXFLAGS)
10_locsvc_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(10_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(10_locsvc_BASE)/generated/Makefile
include $(10_locsvc_BASE)/locservice/Makefile

10_locsvc: $(AREG_OUTPUT_BIN)/$(10_locservice_TARGET_BIN)

.PHONY: 10_locsvc
