15_generated_OUTPUT_OBJ := $(15_pubworker_OUTPUT_OBJ)/generated

15_generated_SRC  :=
15_generated_OBJS :=

include $(15_pubworker_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(15_generated_SRC), $(eval $(call obj, $(cpp), $(15_generated_OUTPUT_OBJ), 15_generated_OBJS, $(15_pubworker_CXXFLAGS))))

DEPS = $(15_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(15_generated_TARGET_BIN): $(15_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
