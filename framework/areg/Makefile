areg_PROJECT_NAME := areg

areg_TARGET_PATH :=
areg_BASE        := $(AREG_BASE)/areg
areg_OUTPUT_OBJ  := $(AREG_OUTPUT_OBJ)/$(areg_PROJECT_NAME)
areg_OUTPUT_LIB  := $(AREG_OUTPUT_LIB)
areg_OUTPUT_BIN  := $(AREG_OUTPUT_BIN)

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
	# TODO: need to verify fPIC on 'windows'
	areg_CXXFLAGS += -fPIC
	ifeq ($(AREG_OS), windows)
		areg_CXXFLAGS += -DEXP_AREG_DLL
	else
		areg_CXXFLAGS += -DEXP_AREG_LIB
	endif
	areg_TARGET_PATH := $(areg_OUTPUT_BIN)/$(AREG_LIB_PREFIX)$(areg_PROJECT_NAME)$(AREG_LIB_EXT)
else
	areg_CXXFLAGS += -DEXP_AREG_LIB
	areg_TARGET_PATH := $(areg_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(areg_PROJECT_NAME)$(AREG_LIB_EXT)
endif


areg: $(areg_TARGET_PATH)

# define one target for each source file
$(foreach cpp, $(areg_SRC), $(eval $(call obj, $(cpp), $(areg_OUTPUT_OBJ), areg_OBJS, $(areg_CXXFLAGS))))

$(areg_TARGET_PATH): $(areg_OBJS)
	@echo "Linking $(areg_PROJECT_NAME) $(AREG_BINARY) library ..."
	@mkdir -p $(dir $@)
	if [ "$(AREG_BINARY)" = "shared" ]; then \
		$(AREG_TOOLCHAIN) -shared $(CXXFLAGS) $(areg_CXXFLAGS) $(areg_OBJS) -o $@; \
	else \
		$(AREG_AR) -rcs $@ $^; \
	fi
