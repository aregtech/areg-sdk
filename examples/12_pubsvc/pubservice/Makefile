12_pubservice_PROJECT_NAME := 12_pubservice

12_pubservice_TARGET_BIN := $(12_pubservice_PROJECT_NAME)$(AREG_BIN_EXT)
12_pubservice_OUTPUT_OBJ := $(12_pubsvc_OUTPUT_OBJ)/pubservice

12_pubservice_SRC  :=
12_pubservice_OBJS :=

include $(12_pubsvc_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(12_pubservice_SRC), $(eval $(call obj, $(cpp), $(12_pubservice_OUTPUT_OBJ), 12_pubservice_OBJS, $(12_pubsvc_CXXFLAGS))))

DEPS = $(12_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(12_pubservice_TARGET_BIN): $(areg_TARGET_PATH) $(12_pubservice_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(12_pubsvc_CXXFLAGS) $(12_pubservice_OBJS) -o $@ $(12_pubsvc_LDFLAGS)
