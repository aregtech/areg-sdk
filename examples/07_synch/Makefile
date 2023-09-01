07_synch_PROJECT_NAME := 07_synch

07_synch_BASE := $(AREG_EXAMPLES)/07_synch

07_synch_TARGET_BIN := $(07_synch_PROJECT_NAME)$(AREG_BIN_EXT)
07_synch_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(07_synch_PROJECT_NAME)

07_synch_SRC  :=
07_synch_OBJS :=

# build list of source files used by 07_synch
include $(07_synch_BASE)/src/Makefile

07_synch: $(AREG_OUTPUT_BIN)/$(07_synch_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(07_synch_SRC), $(eval $(call obj, $(cpp), $(07_synch_OUTPUT_OBJ), 07_synch_OBJS, $(examples_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(07_synch_TARGET_BIN): $(areg_TARGET_PATH) $(07_synch_OBJS) $(AREG_DEPEND)
	@echo "Linking $(07_synch_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(07_synch_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 07_synch
