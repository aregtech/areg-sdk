22_publisher_OUTPUT_OBJ := $(22_pubsub_OUTPUT_OBJ)/publisher

22_publisher_SRC  :=
22_publisher_OBJS :=

include $(22_pubsub_BASE)/publisher/src/Makefile

# define one target for each source file
$(foreach cpp, $(22_publisher_SRC), $(eval $(call obj, $(cpp), $(22_publisher_OUTPUT_OBJ), 22_publisher_OBJS, $(22_pubsub_CXXFLAGS))))

DEPS = $(22_publisher_OBJS:%.o=%.d)
-include $(DEPS)

$(22_publisher_TARGET_BIN): $(areg_TARGET_BIN) $(22_publisher_OBJS) $(22_pubsub_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(22_pubsub_CXXFLAGS) $(22_publisher_OBJS) -o $@ $(22_pubsub_LDFLAGS)
