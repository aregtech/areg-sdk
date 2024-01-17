24_publisher_OUTPUT_OBJ := $(24_pubsubmulti_OUTPUT_OBJ)/publisher

24_publisher_SRC  :=
24_publisher_OBJS :=

include $(24_pubsubmulti_BASE)/publisher/src/Makefile

# define one target for each source file
$(foreach cpp, $(24_publisher_SRC), $(eval $(call obj, $(cpp), $(24_publisher_OUTPUT_OBJ), 24_publisher_OBJS, $(24_pubsubmulti_CXXFLAGS))))

DEPS = $(24_publisher_OBJS:%.o=%.d)
-include $(DEPS)

$(24_publisher_TARGET_BIN): $(areg_TARGET_BIN) $(24_publisher_OBJS) $(24_pubsubmulti_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(24_pubsubmulti_CXXFLAGS) $(24_publisher_OBJS) -o $@ $(24_pubsubmulti_LDFLAGS)
