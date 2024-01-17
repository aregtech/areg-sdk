16_pubclient_OUTPUT_OBJ := $(16_pubfsm_OUTPUT_OBJ)/pubclient

16_pubclient_SRC  :=
16_pubclient_OBJS :=

include $(16_pubfsm_BASE)/pubclient/src/Makefile

# define one target for each source file
$(foreach cpp, $(16_pubclient_SRC), $(eval $(call obj, $(cpp), $(16_pubclient_OUTPUT_OBJ), 16_pubclient_OBJS, $(16_pubfsm_CXXFLAGS))))

DEPS = $(16_pubclient_OBJS:%.o=%.d)
-include $(DEPS)

$(16_pubclient_TARGET_BIN): $(areg_TARGET_BIN) $(16_pubclient_OBJS) $(16_pubfsm_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(16_pubfsm_CXXFLAGS) $(16_pubclient_OBJS) -o $@ $(16_pubfsm_LDFLAGS)
