08_service_PROJECT_NAME := 08_service
08_service_BASE			:= $(AREG_EXAMPLES)/08_service

08_service_TARGET_BIN := $(08_service_PROJECT_NAME)$(AREG_BIN_EXT)
08_service_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(08_service_PROJECT_NAME)

08_service_SRC  :=
08_service_OBJS :=
08_service_CXXFLAGS := -I$(08_service_BASE) $(examples_CXXFLAGS)

# build list of source files used by 08_service
include $(08_service_BASE)/src/Makefile

08_service: $(AREG_OUTPUT_BIN)/$(08_service_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(08_service_SRC), $(eval $(call obj, $(cpp), $(08_service_OUTPUT_OBJ), 08_service_OBJS, $(08_service_CXXFLAGS))))

DEPS = $(08_service_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(08_service_TARGET_BIN): $(areg_TARGET_PATH) $(08_service_OBJS) $(AREG_DEPEND)
	@echo "Linking $(08_service_PROJECT_NAME) ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(examples_CXXFLAGS) $(08_service_OBJS) -o $@ $(examples_LDFLAGS)

.PHONY: 08_service
