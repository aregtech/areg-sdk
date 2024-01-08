
extend_BASE         := $(AREG_BASE)/extend
extend_OUTPUT_OBJ   := $(AREG_OUTPUT_OBJ)/$(extend_PROJECT_NAME)

extend_SRC  :=
extend_OBJS :=

include $(extend_BASE)/console/private/Makefile
include $(extend_BASE)/service/private/Makefile

extend_CXXFLAGS		:= -I$(AREG_BASE) -D$(AREG_LIB_IMPORT)

extend: $(extend_TARGET_BIN)

# define one target for each source file
$(foreach cpp, $(extend_SRC), $(eval $(call obj, $(cpp), $(extend_OUTPUT_OBJ), extend_OBJS, $(extend_CXXFLAGS))))

DEPS = $(extend_OBJS:%.o=%.d)
-include $(DEPS)

$(extend_TARGET_BIN): $(extend_OBJS)
	@echo "Linking AREG extend static library ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
