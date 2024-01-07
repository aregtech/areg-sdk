observer_lib_PROJECT_NAME	:= observerapi

observer_lib_BASE        := $(observer_BASE)/lib
observer_lib_OUTPUT_OBJ  := $(AREG_OUTPUT_OBJ)/$(observer_lib_PROJECT_NAME)

observer_lib_SRC  :=
observer_lib_OBJS :=

include $(observer_lib_BASE)/private/Makefile

observer_lib_CXXFLAGS	:= -I$(AREG_BASE) -D$(AREG_LIB_IMPORT) -fPIC
observer_lib_LDFLAGS	:= $(AREG_TOOL_FLAGS)

ifeq ($(AREG_OBSERVER_LIB), shared)
	observer_lib_CXXFLAGS += -DEXP_OBSERVER_DLL
else
	observer_lib_CXXFLAGS += -DEXP_OBSERVER_LIB
endif

observer_lib_TARGET_BIN := $(AREG_OUTPUT_BIN)/$(AREG_LIB_PREFIX)$(observer_lib_PROJECT_NAME)$(AREG_OBSERVER_LIB_EXT)

observerapi: $(observer_lib_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(observer_lib_SRC), $(eval $(call obj, $(cpp), $(observer_lib_OUTPUT_OBJ), observer_lib_OBJS, $(observer_lib_CXXFLAGS))))

DEPS = $(observer_lib_OBJS:%.o=%.d)
-include $(DEPS)

$(observer_lib_TARGET_BIN): $(AREG_DEPEND) $(observer_lib_OBJS)
	@echo "Linking $(observer_lib_PROJECT_NAME) $(AREG_OBSERVER_LIB) library ..."
	@mkdir -p $(dir $@)
	if [ "$(AREG_OBSERVER_LIB)" = "shared" ]; then \
		$(AREG_TOOLCHAIN) -shared $(CXXFLAGS) $(observer_lib_CXXFLAGS) $(observer_lib_OBJS) -o $@  $(observer_lib_LDFLAGS); \
	else \
		$(AREG_AR) -rcs $@ $^; \
	fi
