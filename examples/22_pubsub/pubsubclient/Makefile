22_pubsubclient_PROJECT_NAME := 22_pubsubclient

22_pubsubclient_TARGET_BIN := $(22_pubsubclient_PROJECT_NAME)$(AREG_BIN_EXT)
22_pubsubclient_OUTPUT_OBJ := $(22_pubsub_OUTPUT_OBJ)/pubsubclient

22_pubsubclient_SRC  :=
22_pubsubclient_OBJS :=

include $(22_pubsub_BASE)/pubsubclient/src/Makefile

# define one target for each source file
$(foreach cpp, $(22_pubsubclient_SRC), $(eval $(call obj, $(cpp), $(22_pubsubclient_OUTPUT_OBJ), 22_pubsubclient_OBJS, $(22_pubsub_CXXFLAGS))))

DEPS = $(22_pubsubclient_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(22_pubsubclient_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(22_generated_TARGET_BIN) $(22_pubsubclient_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(22_pubsub_CXXFLAGS) $(22_pubsubclient_OBJS) $(22_pubsub_LDFLAGS) -o $@
