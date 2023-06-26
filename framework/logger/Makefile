logger_PROJECT_NAME := logger

logger_BASE := $(AREG_BASE)/logger

logger_TARGET_BIN  :=

logger_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(logger_PROJECT_NAME)

logger_SRC  :=
logger_OBJS := 

include $(logger_BASE)/app/private/Makefile

logger_LDFLAGS  :=
logger_CXXFLAGS :=
ifeq ($(AREG_BINARY), shared)
	logger_CXXFLAGS += -DIMP_AREG_DLL
else
	logger_CXXFLAGS += -DIMP_AREG_LIB
endif

logger_LDFLAGS  += $(AREG_LIB_INCLUDES) -lareg-extensions -lareg $(LDFLAGS) $(AREG_EXTENDED_LIBS)

logger_TARGET_BIN := $(logger_PROJECT_NAME)$(AREG_BIN_EXT)

logger: $(AREG_OUTPUT_BIN)/$(logger_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(logger_SRC), $(eval $(call obj, $(cpp), $(logger_OUTPUT_OBJ), logger_OBJS, $(logger_CXXFLAGS))))

DEPS = $(logger_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(logger_TARGET_BIN): $(areg_TARGET_PATH) $(logger_OBJS)
	@echo "Linking logger ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(logger_CXXFLAGS) $(logger_OBJS) -o $@ $(AREG_EXTENDED_LIBS) $(logger_LDFLAGS) $(LDFLAGS)
