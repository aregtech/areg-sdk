
logobserverapi_OUTPUT_OBJ  := $(AREG_OUTPUT_OBJ)/$(logobserverapi_PROJECT_NAME)

logobserverapi_SRC  :=
logobserverapi_OBJS :=

include $(logobserver_BASE)/lib/private/Makefile

logobserverapi_CXXFLAGS		:= -I$(AREG_BASE) -D$(AREG_LIB_IMPORT) -fPIC
logobserverapi_LDFLAGS		:= $(AREG_TOOL_FLAGS)

ifeq ($(AREG_LOGOBSERVER_LIB), shared)
	OBSERVER_API_IMPORT		:= -DIMP_LOGOBSERVER_DLL
	logobserverapi_CXXFLAGS	+= -DEXP_LOGOBSERVER_DLL
	OBSERVER_API_PATH		:= $(AREG_OUTPUT_BIN)
else
	OBSERVER_API_IMPORT		:= -DIMP_LOGOBSERVER_LIB
	logobserverapi_CXXFLAGS	+= -DEXP_LOGOBSERVER_LIB
	OBSERVER_API_PATH		:= $(AREG_OUTPUT_LIB)
endif

logobserverapi_TARGET_BIN	:= $(OBSERVER_API_PATH)/$(AREG_LIB_PREFIX)$(logobserverapi_PROJECT_NAME)$(AREG_LOGOBSERVERAPI_EXT)

logobserverapi: $(logobserverapi_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(logobserverapi_SRC), $(eval $(call obj, $(cpp), $(logobserverapi_OUTPUT_OBJ), logobserverapi_OBJS, $(logobserverapi_CXXFLAGS))))

DEPS = $(logobserverapi_OBJS:%.o=%.d)
-include $(DEPS)

$(logobserverapi_TARGET_BIN): $(logobserverapi_OBJS) $(AREG_DEPEND) 
	@echo "Linking AREG SDK $(logobserverapi_PROJECT_NAME) $(AREG_LOGOBSERVER_LIB) library ..."
	@mkdir -p $(dir $@)
	if [ "$(AREG_LOGOBSERVER_LIB)" = "shared" ]; then \
		$(AREG_TOOLCHAIN) -shared $(CXXFLAGS) $(logobserverapi_CXXFLAGS) $(logobserverapi_OBJS) -o $@  $(logobserverapi_LDFLAGS); \
	else \
		$(AREG_AR) -rcs $@ $^; \
	fi
