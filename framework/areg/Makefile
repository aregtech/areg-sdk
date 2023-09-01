areg_PROJECT_NAME := $(AREG_PROJECT)

areg_TARGET_PATH :=
areg_BASE        := $(AREG_BASE)/areg
areg_OUTPUT_OBJ  := $(AREG_OUTPUT_OBJ)/$(areg_PROJECT_NAME)

areg_SRC  :=
areg_OBJS :=

include $(areg_BASE)/appbase/private/Makefile
include $(areg_BASE)/base/private/Makefile
include $(areg_BASE)/component/private/Makefile
include $(areg_BASE)/ipc/private/Makefile
include $(areg_BASE)/persist/private/Makefile
include $(areg_BASE)/trace/private/Makefile

areg_CXXFLAGS :=
ifeq ($(AREG_BINARY), shared)
	areg_CXXFLAGS += -fPIC -DEXP_AREG_DLL
	areg_TARGET_PATH := $(AREG_OUTPUT_BIN)/$(AREG_LIB_PREFIX)$(areg_PROJECT_NAME)$(AREG_LIB_EXT)
else
	areg_CXXFLAGS += -DEXP_AREG_LIB
	areg_TARGET_PATH := $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(areg_PROJECT_NAME)$(AREG_LIB_EXT)
endif

areg: $(areg_TARGET_PATH)

# define one target for each source file
$(foreach cpp, $(areg_SRC), $(eval $(call obj, $(cpp), $(areg_OUTPUT_OBJ), areg_OBJS, $(areg_CXXFLAGS))))

DEPS = $(areg_OBJS:%.o=%.d)
-include $(DEPS)

$(areg_TARGET_PATH): $(areg_OBJS)
	@echo "Linking $(areg_PROJECT_NAME) $(AREG_BINARY) library ..."
	@mkdir -p $(dir $@)
	if [ "$(AREG_BINARY)" = "shared" ]; then \
		$(AREG_TOOLCHAIN) -shared $(CXXFLAGS) $(areg_CXXFLAGS) $(areg_OBJS) -o $@  $(LDFLAGS); \
	else \
		$(AREG_AR) -rcs $@ $^; \
	fi
	@mkdir -p $(AREG_OUTPUT_BIN)/config
	@cp $(areg_BASE)/resources/log.init $(areg_BASE)/resources/router.init $(areg_BASE)/resources/logger.init $(AREG_OUTPUT_BIN)/config/
