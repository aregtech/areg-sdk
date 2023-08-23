extend_PROJECT_NAME := areg-extend

extend_TARGET_BIN   := $(AREG_LIB_PREFIX)$(extend_PROJECT_NAME)$(AREG_STATIC_LIB)
extend_BASE         := $(AREG_BASE)/extend
extend_OUTPUT_OBJ   := $(AREG_OUTPUT_OBJ)/$(extend_PROJECT_NAME)
extend_TARGET_PATH  := $(AREG_OUTPUT_LIB)/$(extend_TARGET_BIN)

ifeq ($(AREG_BINARY), shared)
    extend_CXXFLAGS += -DIMP_AREG_DLL
else
    extend_CXXFLAGS += -DIMP_AREG_LIB
endif

extend_SRC  :=
extend_OBJS :=

include $(extend_BASE)/console/private/Makefile

extend: $(extend_TARGET_PATH)

# define one target for each source file
$(foreach cpp, $(extend_SRC), $(eval $(call obj, $(cpp), $(extend_OUTPUT_OBJ), extend_OBJS, $(extend_CXXFLAGS))))

DEPS = $(extend_OBJS:%.o=%.d)
-include $(DEPS)

$(extend_TARGET_PATH): $(extend_OBJS)
	@echo "Linking AREG extend static library ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
