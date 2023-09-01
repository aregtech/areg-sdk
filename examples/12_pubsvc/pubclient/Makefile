12_pubclient_PROJECT_NAME := 12_pubclient

12_pubclient_TARGET_BIN := $(12_pubclient_PROJECT_NAME)$(AREG_BIN_EXT)
12_pubclient_OUTPUT_OBJ := $(12_pubsvc_OUTPUT_OBJ)/pubclient

12_pubclient_SRC  :=
12_pubclient_OBJS :=

include $(12_pubsvc_BASE)/pubclient/src/Makefile

# define one target for each source file
$(foreach cpp, $(12_pubclient_SRC), $(eval $(call obj, $(cpp), $(12_pubclient_OUTPUT_OBJ), 12_pubclient_OBJS, $(12_pubsvc_CXXFLAGS))))

DEPS = $(12_pubclient_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(12_pubclient_TARGET_BIN): $(areg_TARGET_PATH) $(12_pubclient_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(12_pubsvc_CXXFLAGS) $(12_pubclient_OBJS) -o $@ $(12_pubsvc_LDFLAGS)
