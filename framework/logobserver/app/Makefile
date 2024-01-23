logobserver_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(logobserver_PROJECT_NAME)

logobserver_SRC  :=
logobserver_OBJS := 

include $(logobserver_BASE)/app/private/Makefile

logobserver_CXXFLAGS	:= $(AREG_COMMON_INCLUDES) -D$(AREG_LIB_IMPORT) $(OBSERVER_API_IMPORT)
logobserver_LDFLAGS		:= $(AREG_TOOL_FLAGS) -l$(logobserverapi_PROJECT_NAME) -lsqlite3

logobserver: $(logobserver_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(logobserver_SRC), $(eval $(call obj, $(cpp), $(logobserver_OUTPUT_OBJ), logobserver_OBJS, $(logobserver_CXXFLAGS))))

DEPS = $(logobserver_OBJS:%.o=%.d)
-include $(DEPS)

$(logobserver_TARGET_BIN): $(areg_TARGET_BIN) $(logobserver_OBJS) $(AREG_DEPEND) $(logobserverapi_TARGET_BIN)
	@echo "Linking log observer ..."
	@mkdir -p $(dir $@)
	$(AREG_CXX_TOOLCHAIN) $(CXXFLAGS) $(logobserver_CXXFLAGS) $(logobserver_OBJS) -o $@ $(logobserver_LDFLAGS)
