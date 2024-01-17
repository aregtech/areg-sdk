22_subscriber_OUTPUT_OBJ := $(22_pubsub_OUTPUT_OBJ)/subscriber

22_subscriber_SRC  :=
22_subscriber_OBJS :=

include $(22_pubsub_BASE)/subscriber/src/Makefile

# define one target for each source file
$(foreach cpp, $(22_subscriber_SRC), $(eval $(call obj, $(cpp), $(22_subscriber_OUTPUT_OBJ), 22_subscriber_OBJS, $(22_pubsub_CXXFLAGS))))

DEPS = $(22_subscriber_OBJS:%.o=%.d)
-include $(DEPS)

$(22_subscriber_TARGET_BIN): $(areg_TARGET_BIN) $(22_subscriber_OBJS) $(22_pubsub_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(22_pubsub_CXXFLAGS) $(22_subscriber_OBJS) -o $@ $(22_pubsub_LDFLAGS)
