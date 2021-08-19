12_ipcsvchello_PROJECT_NAME := 12_ipcsvchello

12_ipcsvchello_BASE := $(12_pubsvchello_BASE)/ipcsvchello

12_ipcsvchello_TARGET_BIN := $(12_ipcsvchello_PROJECT_NAME)$(AREG_BIN_EXT)
12_ipcsvchello_OUTPUT_OBJ := $(12_pubsvchello_OUTPUT_OBJ)/$(12_ipcsvchello_PROJECT_NAME)

12_ipcsvchello_SRC  :=
12_ipcsvchello_OBJS :=

include $(12_ipcsvchello_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(12_ipcsvchello_SRC), $(eval $(call obj, $(cpp), $(12_ipcsvchello_OUTPUT_OBJ), 12_ipcsvchello_OBJS, $(12_pubsvchello_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(12_ipcsvchello_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(12_generated_TARGET_BIN) $(12_ipcsvchello_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(12_pubsvchello_CXXFLAGS) $(12_ipcsvchello_OBJS) $(12_pubsvchello_LDFLAGS) -o $@
