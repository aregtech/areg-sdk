16_pubservice_OUTPUT_OBJ := $(16_pubfsm_OUTPUT_OBJ)/pubservice

16_pubservice_SRC  :=
16_pubservice_OBJS :=

include $(16_pubfsm_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(16_pubservice_SRC), $(eval $(call obj, $(cpp), $(16_pubservice_OUTPUT_OBJ), 16_pubservice_OBJS, $(16_pubfsm_CXXFLAGS))))

DEPS = $(16_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(16_pubservice_TARGET_BIN): $(areg_TARGET_BIN) $(16_pubservice_OBJS) $(16_pubfsm_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(16_pubfsm_CXXFLAGS) $(16_pubservice_OBJS) -o $@ $(16_pubfsm_LDFLAGS)
