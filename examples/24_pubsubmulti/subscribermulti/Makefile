24_subscribermulti_OUTPUT_OBJ := $(24_pubsubmulti_OUTPUT_OBJ)/subscriber

24_subscribermulti_SRC  :=
24_subscribermulti_OBJS :=

include $(24_pubsubmulti_BASE)/subscribermulti/src/Makefile

# define one target for each source file
$(foreach cpp, $(24_subscribermulti_SRC), $(eval $(call obj, $(cpp), $(24_subscribermulti_OUTPUT_OBJ), 24_subscribermulti_OBJS, $(24_pubsubmulti_CXXFLAGS))))

DEPS = $(24_subscribermulti_OBJS:%.o=%.d)
-include $(DEPS)

$(24_subscribermulti_TARGET_BIN): $(areg_TARGET_BIN) $(24_subscribermulti_OBJS) $(24_pubsubmulti_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(24_pubsubmulti_CXXFLAGS) $(24_subscribermulti_OBJS) -o $@ $(24_pubsubmulti_LDFLAGS)
