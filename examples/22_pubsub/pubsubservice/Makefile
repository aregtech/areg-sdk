22_pubsubservice_PROJECT_NAME := 22_pubsubservice

22_pubsubservice_TARGET_BIN := $(22_pubsubservice_PROJECT_NAME)$(AREG_BIN_EXT)
22_pubsubservice_OUTPUT_OBJ := $(22_pubsub_OUTPUT_OBJ)/$(22_pubsubservice_PROJECT_NAME)

22_pubsubservice_SRC  :=
22_pubsubservice_OBJS :=

include $(22_pubsub_BASE)/pubsubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(22_pubsubservice_SRC), $(eval $(call obj, $(cpp), $(22_pubsubservice_OUTPUT_OBJ), 22_pubsubservice_OBJS, $(22_pubsub_CXXFLAGS))))

DEPS = $(22_pubsubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(22_pubsubservice_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(22_generated_TARGET_BIN) $(22_pubsubservice_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(22_pubsub_CXXFLAGS) $(22_pubsubservice_OBJS) $(22_pubsub_LDFLAGS) -o $@
