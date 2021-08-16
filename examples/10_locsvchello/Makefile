10_locsvchello_BASE       := $(AREG_EXAMPLES)/10_locsvchello
10_locsvchello_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/10_locsvchello

10_locsvchello_CXXFLAGS   = -I$(10_locsvchello_BASE) $(examples_CXXFLAGS)
10_locsvchello_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(10_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(10_locsvchello_BASE)/generated/Makefile
include $(10_locsvchello_BASE)/svchello/Makefile

10_locsvchello: $(AREG_OUTPUT_BIN)/$(10_svchello_TARGET_BIN)

.PHONY: 10_locsvchello
