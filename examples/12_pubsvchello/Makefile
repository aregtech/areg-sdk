12_pubsvchello_BASE       := $(AREG_EXAMPLES)/12_pubsvchello
12_pubsvchello_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/12_pubsvchello

12_pubsvchello_CXXFLAGS   = -I$(12_pubsvchello_BASE) $(examples_CXXFLAGS)
12_pubsvchello_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(12_generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(12_pubsvchello_BASE)/generated/Makefile
include $(12_pubsvchello_BASE)/ipcsvchello/Makefile
include $(12_pubsvchello_BASE)/ipcusrhello/Makefile

12_pubsvchello: $(AREG_OUTPUT_BIN)/$(12_ipcsvchello_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(12_ipcusrhello_TARGET_BIN)

.PHONY: 12_pubsvchello
