14_pubservice_OUTPUT_OBJ := $(14_pubtraffic_OUTPUT_OBJ)/pubservice

14_pubservice_SRC  :=
14_pubservice_OBJS :=

include $(14_pubtraffic_BASE)/pubservice/src/Makefile

# define one target for each source file
$(foreach cpp, $(14_pubservice_SRC), $(eval $(call obj, $(cpp), $(14_pubservice_OUTPUT_OBJ), 14_pubservice_OBJS, $(14_pubtraffic_CXXFLAGS))))

DEPS = $(14_pubservice_OBJS:%.o=%.d)
-include $(DEPS)

$(14_pubservice_TARGET_BIN): $(areg_TARGET_BIN) $(14_pubservice_OBJS) $(14_pubtraffic_DEPEND)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(14_pubtraffic_CXXFLAGS) $(14_pubservice_OBJS) -o $@ $(14_pubtraffic_LDFLAGS)
