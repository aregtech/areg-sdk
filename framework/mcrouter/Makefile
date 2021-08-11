mcrouter_PROJECT_NAME := mcrouter

mcrouter_BASE := $(AREG_BASE)/mcrouter

mcrouter_TARGET_BIN  :=

mcrouter_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(mcrouter_PROJECT_NAME)
mcrouter_OUTPUT_LIB := $(AREG_OUTPUT_LIB)
mcrouter_OUTPUT_BIN := $(AREG_OUTPUT_BIN)

mcrouter_SRC  :=
mcrouter_OBJS := 

include $(mcrouter_BASE)/app/private/Makefile
include $(mcrouter_BASE)/tcp/private/Makefile

mcrouter_LDFLAGS :=
mcrouter_CXXFLAGS :=
ifeq ($(AREG_BINARY), shared)
	mcrouter_CXXFLAGS += -DIMP_AREG_DLL
	mcrouter_LDFLAGS  += -Wl,-rpath=$(mcrouter_OUTPUT_BIN) -L $(mcrouter_OUTPUT_BIN) -lareg $(LDFLAGS)
else
	mcrouter_CXXFLAGS += -DIMP_AREG_LIB
	mcrouter_LDFLAGS  += -L $(mcrouter_OUTPUT_LIB) -Wl,-Bstatic -lareg -Wl,-Bdynamic $(LDFLAGS)
endif

mcrouter_TARGET_BIN := $(mcrouter_PROJECT_NAME)$(AREG_BIN_EXT)

mcrouter: $(mcrouter_OUTPUT_BIN)/$(mcrouter_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(mcrouter_SRC), $(eval $(call obj, $(cpp), $(mcrouter_OUTPUT_OBJ), mcrouter_OBJS, $(mcrouter_CXXFLAGS))))

$(mcrouter_OUTPUT_BIN)/$(mcrouter_TARGET_BIN): $(areg_TARGET_PATH) $(mcrouter_OBJS)
	@echo "Linking mcrouter ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(mcrouter_OBJS) $(mcrouter_LDFLAGS) -o $@
