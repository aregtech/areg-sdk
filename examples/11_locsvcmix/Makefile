11_locsvcmix_BASE       := $(AREG_EXAMPLES)/11_locsvcmix
11_locsvcmix_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/11_locsvcmix

11_locsvcmix_CXXFLAGS   = -I$(11_locsvcmix_BASE) $(examples_CXXFLAGS)
11_locsvcmix_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(11_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(11_locsvcmix_BASE)/generated/Makefile
include $(11_locsvcmix_BASE)/svcmix/Makefile

11_locsvcmix: $(AREG_OUTPUT_BIN)/$(11_svcmix_TARGET_BIN)

.PHONY: 11_locsvcmix
