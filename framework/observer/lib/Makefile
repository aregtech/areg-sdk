observer_lib_PROJECT_NAME	:= observerapi

observer_lib_BASE        := $(observer_BASE)/lib
observer_lib_TARGET_PATH :=
observer_lib_OUTPUT_OBJ  := $(AREG_OUTPUT_OBJ)/$(observer_lib_PROJECT_NAME)

observer_lib_SRC  :=
observer_lib_OBJS :=

include $(observer_lib_BASE)/private/Makefile

observer_lib_CXXFLAGS	:= -I$(AREG_BASE)
observer_lib_LDFLAGS	:= $(AREG_TOOL_FLAGS)

ifeq ($(AREG_OBSERVER_LIB), shared)
	ifeq ($(AREG_BINARY), shared)
		observer_lib_CXXFLAGS += -DIMP_AREG_DLL
	else
		observer_lib_CXXFLAGS += -DIMP_AREG_LIB
	endif

	observer_lib_CXXFLAGS += -fPIC -DEXP_OBSERVER_DLL
	observer_lib_TARGET_PATH := $(AREG_OUTPUT_BIN)/$(AREG_LIB_PREFIX)$(observer_lib_PROJECT_NAME)$(AREG_OBSERVER_LIB_EXT)
else
	ifeq ($(AREG_BINARY), shared)
		observer_lib_CXXFLAGS += -DIMP_AREG_DLL
	else
		observer_lib_CXXFLAGS += -DIMP_AREG_LIB
	endif
	observer_lib_CXXFLAGS += -fPIC -DEXP_OBSERVER_LIB
	observer_lib_TARGET_PATH := $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(observer_lib_PROJECT_NAME)$(AREG_OBSERVER_LIB_EXT)
endif

observerapi: $(observer_lib_TARGET_PATH)

# define one target for each source file
$(foreach cpp, $(observer_lib_SRC), $(eval $(call obj, $(cpp), $(observer_lib_OUTPUT_OBJ), observer_lib_OBJS, $(observer_lib_CXXFLAGS))))

DEPS = $(observer_lib_OBJS:%.o=%.d)
-include $(DEPS)

$(observer_lib_TARGET_PATH): $(AREG_DEPEND) $(observer_lib_OBJS)
	@echo "Linking $(observer_lib_PROJECT_NAME) $(AREG_OBSERVER_LIB) library ..."
	@mkdir -p $(dir $@)
	if [ "$(AREG_OBSERVER_LIB)" = "shared" ]; then \
		$(AREG_TOOLCHAIN) -shared $(CXXFLAGS) $(observer_lib_CXXFLAGS) $(observer_lib_OBJS) -o $@  $(observer_lib_LDFLAGS); \
	else \
		$(AREG_AR) -rcs $@ $^; \
	fi
