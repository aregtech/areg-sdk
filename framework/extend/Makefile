extend_PROJECT_NAME := $(AREG_EXTENDED_PROJECT)

extend_BASE         := $(AREG_BASE)/extend
extend_TARGET_LIB   := $(AREG_EXTEND_LIB)
extend_OUTPUT_OBJ   := $(AREG_OUTPUT_OBJ)/$(extend_PROJECT_NAME)

extend_SRC  :=
extend_OBJS :=

include $(extend_BASE)/console/private/Makefile
include $(extend_BASE)/service/private/Makefile


extend_CXXFLAGS		:= 
ifeq ($(AREG_BINARY), shared)
    extend_CXXFLAGS := -I$(AREG_BASE) -DIMP_AREG_DLL
else
    extend_CXXFLAGS := -I$(AREG_BASE) -DIMP_AREG_LIB
endif

extend: $(extend_TARGET_PATH)

# define one target for each source file
$(foreach cpp, $(extend_SRC), $(eval $(call obj, $(cpp), $(extend_OUTPUT_OBJ), extend_OBJS, $(extend_CXXFLAGS))))

DEPS = $(extend_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(extend_TARGET_LIB): $(extend_OBJS)
	@echo "Linking AREG extend static library ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
