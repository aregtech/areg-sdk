15_pubservice_OUTPUT_OBJ := $(15_pubworker_OUTPUT_OBJ)/pubservice

15_pubservice_SRC  :=
15_pubservice_OBJS :=

include $(15_pubworker_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(15_pubservice_SRC), $(eval $(call obj, $(cpp), $(15_pubservice_OUTPUT_OBJ), 15_pubservice_OBJS, $(15_pubworker_CXXFLAGS))))

DEPS = $(15_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(15_pubservice_TARGET_BIN): $(areg_TARGET_PATH) $(15_pubservice_OBJS) $(15_pubworker_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(15_pubworker_CXXFLAGS) $(15_pubservice_OBJS) -o $@ $(15_pubworker_LDFLAGS)
