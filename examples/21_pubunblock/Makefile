21_pubunblock_BASE       := $(AREG_EXAMPLES)/21_pubunblock
21_pubunblock_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/21_pubunblock

21_pubunblock_CXXFLAGS   = -I$(21_pubunblock_BASE) $(examples_CXXFLAGS)
21_pubunblock_LDFLAGS    = $(AREG_LIB_INCLUDES) -l$(21_generated_PROJECT_NAME) -lareg $(LDFLAGS)

# 'generated' have to be first
include $(21_pubunblock_BASE)/generated/Makefile
include $(21_pubunblock_BASE)/pubservice/Makefile
include $(21_pubunblock_BASE)/pubclient/Makefile

21_pubunblock: $(AREG_OUTPUT_BIN)/$(21_pubclient_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(21_pubservice_TARGET_BIN)

.PHONY: 21_pubunblock
