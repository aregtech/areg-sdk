05_timer_PROJECT_NAME := 05_timer

05_timer_BASE := $(AREG_EXAMPLES)/05_timer

05_timer_TARGET_BIN := $(05_timer_PROJECT_NAME)$(AREG_BIN_EXT)
05_timer_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(05_timer_PROJECT_NAME)

05_timer_SRC  :=
05_timer_OBJS :=

# build list of source files used by 05_timer
include $(05_timer_BASE)/src/Makefile

05_timer: $(AREG_OUTPUT_BIN)/$(05_timer_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(05_timer_SRC), $(eval $(call obj, $(cpp), $(05_timer_OUTPUT_OBJ), 05_timer_OBJS, $(examples_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(05_timer_TARGET_BIN): $(areg_TARGET_PATH) $(05_timer_OBJS)
	@echo "Linking $(05_timer_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(05_timer_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 05_timer
