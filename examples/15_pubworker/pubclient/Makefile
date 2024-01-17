15_pubclient_OUTPUT_OBJ := $(15_pubworker_OUTPUT_OBJ)/pubclient

15_pubclient_SRC  :=
15_pubclient_OBJS :=

include $(15_pubworker_BASE)/pubclient/src/Makefile

# define one target for each source file
$(foreach cpp, $(15_pubclient_SRC), $(eval $(call obj, $(cpp), $(15_pubclient_OUTPUT_OBJ), 15_pubclient_OBJS, $(15_pubworker_CXXFLAGS))))

DEPS = $(15_pubclient_OBJS:%.o=%.d)
-include $(DEPS)

$(15_pubclient_TARGET_BIN): $(areg_TARGET_BIN) $(15_pubclient_OBJS) $(15_pubworker_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(15_pubworker_CXXFLAGS) $(15_pubclient_OBJS) -o $@ $(15_pubworker_LDFLAGS)
