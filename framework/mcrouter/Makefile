
mcrouter_BASE       := $(AREG_BASE)/mcrouter
mcrouter_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(mcrouter_PROJECT_NAME)

mcrouter_SRC  :=
mcrouter_OBJS := 

include $(mcrouter_BASE)/app/private/Makefile
include $(mcrouter_BASE)/tcp/private/Makefile

mcrouter_CXXFLAGS	:= -I$(AREG_BASE) -D$(AREG_LIB_IMPORT)
mcrouter_LDFLAGS	:= $(AREG_TOOL_FLAGS)

mcrouter: $(mcrouter_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(mcrouter_SRC), $(eval $(call obj, $(cpp), $(mcrouter_OUTPUT_OBJ), mcrouter_OBJS, $(mcrouter_CXXFLAGS))))

DEPS = $(mcrouter_OBJS:%.o=%.d)
-include $(DEPS)

$(mcrouter_TARGET_BIN): $(areg_TARGET_BIN) $(mcrouter_OBJS) $(AREG_DEPEND)
	@echo "Linking AREG SDK mcrouter ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(mcrouter_CXXFLAGS) $(mcrouter_OBJS) -o $@ $(mcrouter_LDFLAGS)
