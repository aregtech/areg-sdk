15_ipcsvcpatient_PROJECT_NAME := 15_ipcsvcpatient

15_ipcsvcpatient_BASE := $(15_pubsvcworker_BASE)/ipcsvcpatient

15_ipcsvcpatient_TARGET_BIN := $(15_ipcsvcpatient_PROJECT_NAME)$(AREG_BIN_EXT)
15_ipcsvcpatient_OUTPUT_OBJ := $(15_pubsvcworker_OUTPUT_OBJ)/$(15_ipcsvcpatient_PROJECT_NAME)

15_ipcsvcpatient_SRC  :=
15_ipcsvcpatient_OBJS :=

include $(15_ipcsvcpatient_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(15_ipcsvcpatient_SRC), $(eval $(call obj, $(cpp), $(15_ipcsvcpatient_OUTPUT_OBJ), 15_ipcsvcpatient_OBJS, $(15_pubsvcworker_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(15_ipcsvcpatient_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(15_generated_TARGET_BIN) $(15_ipcsvcpatient_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(15_pubsvcworker_CXXFLAGS) $(15_ipcsvcpatient_OBJS) $(15_pubsvcworker_LDFLAGS) -o $@
