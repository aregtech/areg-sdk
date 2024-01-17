09_svcmulti_PROJECT_NAME:= 09_svcmulti
09_svcmulti_BASE		:= $(AREG_EXAMPLES)/09_svcmulti
09_svcmulti_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(09_svcmulti_PROJECT_NAME)$(AREG_BIN_EXT)
09_svcmulti_OUTPUT_OBJ	:= $(AREG_OUTPUT_OBJ)/$(09_svcmulti_PROJECT_NAME)

09_svcmulti_SRC  :=
09_svcmulti_OBJS :=
09_svcmulti_CXXFLAGS := -I$(09_svcmulti_BASE) $(examples_CXXFLAGS)

# build list of source files used by 09_svcmulti
include $(09_svcmulti_BASE)/src/Makefile

09_svcmulti: $(09_svcmulti_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(09_svcmulti_SRC), $(eval $(call obj, $(cpp), $(09_svcmulti_OUTPUT_OBJ), 09_svcmulti_OBJS, $(09_svcmulti_CXXFLAGS))))

DEPS = $(09_svcmulti_OBJS:%.o=%.d)
-include $(DEPS)

$(09_svcmulti_TARGET_BIN): $(areg_TARGET_BIN) $(09_svcmulti_OBJS) $(AREG_DEPEND)
	@echo "Linking $(09_svcmulti_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(09_svcmulti_OBJS) $(examples_LDFLAGS) -o $@

.PHONY: 09_svcmulti
