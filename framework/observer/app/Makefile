logobserver_PROJECT_NAME := logobserver

logobserver_BASE       := $(AREG_BASE)/observer
logobserver_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(logobserver_PROJECT_NAME)

logobserver_SRC  :=
logobserver_OBJS := 

include $(logobserver_BASE)/app/private/Makefile

logobserver_CXXFLAGS	:= -I$(AREG_BASE) -D$(AREG_LIB_IMPORT)
logobserver_LDFLAGS		:= $(AREG_TOOL_FLAGS) -l$(observer_lib_PROJECT_NAME) -lareg-extend
logobserver_TARGET_BIN	:= $(AREG_OUTPUT_BIN)/$(logobserver_PROJECT_NAME)$(AREG_BIN_EXT)

logobserver: $(logobserver_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(logobserver_SRC), $(eval $(call obj, $(cpp), $(logobserver_OUTPUT_OBJ), logobserver_OBJS, $(logobserver_CXXFLAGS))))

DEPS = $(logobserver_OBJS:%.o=%.d)
-include $(DEPS)

$(logobserver_TARGET_BIN): $(areg_TARGET_PATH) $(logobserver_OBJS) $(AREG_DEPEND) $(observer_lib_TARGET_BIN)
	@echo "Linking log observer ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(logobserver_CXXFLAGS) $(logobserver_OBJS) -o $@ $(logobserver_LDFLAGS)
