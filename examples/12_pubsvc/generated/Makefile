12_generated_OUTPUT_OBJ := $(12_pubsvc_OUTPUT_OBJ)/generated

12_generated_SRC  :=
12_generated_OBJS :=

include $(12_pubsvc_BASE)/generated/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(12_generated_SRC), $(eval $(call obj, $(cpp), $(12_generated_OUTPUT_OBJ), 12_generated_OBJS, $(12_pubsvc_CXXFLAGS))))

DEPS = $(12_generated_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(12_generated_TARGET_BIN): $(12_generated_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
