22_subscriber_PROJECT_NAME := 22_subscriber

22_subscriber_TARGET_BIN := $(22_subscriber_PROJECT_NAME)$(AREG_BIN_EXT)
22_subscriber_OUTPUT_OBJ := $(22_pubsub_OUTPUT_OBJ)/subscriber

22_subscriber_SRC  :=
22_subscriber_OBJS :=

include $(22_pubsub_BASE)/subscriber/src/Makefile

# define one target for each source file
$(foreach cpp, $(22_subscriber_SRC), $(eval $(call obj, $(cpp), $(22_subscriber_OUTPUT_OBJ), 22_subscriber_OBJS, $(22_pubsub_CXXFLAGS))))

DEPS = $(22_subscriber_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(22_subscriber_TARGET_BIN): $(areg_TARGET_PATH) $(22_subscriber_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(22_pubsub_CXXFLAGS) $(22_subscriber_OBJS) -o $@ $(22_pubsub_LDFLAGS)
