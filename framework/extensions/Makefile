extensions_PROJECT_NAME := areg-extensions

extensions_TARGET_BIN   := $(AREG_LIB_PREFIX)$(extensions_PROJECT_NAME)$(AREG_STATIC_LIB)
extensions_BASE         := $(AREG_BASE)/extensions
extensions_OUTPUT_OBJ   := $(AREG_OUTPUT_OBJ)/$(extensions_PROJECT_NAME)
extensions_TARGET_PATH  :=

ifeq ($(AREG_BINARY), shared)
    extensions_CXXFLAGS += -DIMP_AREG_DLL
    extensions_LDFLAGS  += -Wl,-rpath=$(AREG_OUTPUT_BIN) -L$(AREG_OUTPUT_BIN) -lareg -lncurses $(LDFLAGS)
else
    extensions_CXXFLAGS += -DIMP_AREG_LIB
    extensions_LDFLAGS  += -L$(AREG_OUTPUT_LIB) -Wl,-Bstatic -lareg -lncurses -Wl,-Bdynamic $(LDFLAGS)
endif

extensions_SRC  :=
extensions_OBJS :=

include $(extensions_BASE)/console/private/Makefile

extensions: $(AREG_OUTPUT_LIB)/$(extensions_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(extensions_SRC), $(eval $(call obj, $(cpp), $(extensions_OUTPUT_OBJ), extensions_OBJS, $(extensions_CXXFLAGS))))

DEPS = $(extensions_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(extensions_TARGET_BIN): $(extensions_OBJS)
	@echo "Linking AREG extensions library ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
