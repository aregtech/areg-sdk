logger_PROJECT_NAME := logger

logger_BASE       := $(AREG_BASE)/logger
logger_TARGET_BIN :=
logger_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(logger_PROJECT_NAME)

logger_SRC  :=
logger_OBJS := 

include $(logger_BASE)/app/private/Makefile
include $(logger_BASE)/service/private/Makefile

logger_CXXFLAGS		:= -I$(AREG_BASE) $(AREG_LIB_IMPORT)
logger_LDFLAGS		:= $(AREG_TOOL_FLAGS)
logger_TARGET_BIN	:= $(logger_PROJECT_NAME)$(AREG_BIN_EXT)

logger: $(AREG_OUTPUT_BIN)/$(logger_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(logger_SRC), $(eval $(call obj, $(cpp), $(logger_OUTPUT_OBJ), logger_OBJS, $(logger_CXXFLAGS))))

DEPS = $(logger_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(logger_TARGET_BIN): $(areg_TARGET_PATH) $(logger_OBJS) $(AREG_DEPEND)
	@echo "Linking logger ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(logger_CXXFLAGS) $(logger_OBJS) -o $@ $(logger_LDFLAGS)
