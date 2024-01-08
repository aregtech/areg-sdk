23_common_OUTPUT_OBJ := $(23_pubsubmix_OUTPUT_OBJ)/common

23_common_SRC  :=
23_common_OBJS :=

include $(23_pubsubmix_BASE)/common/src/Makefile

# define one target for each source file
$(foreach cpp, $(23_common_SRC), $(eval $(call obj, $(cpp), $(23_common_OUTPUT_OBJ), 23_common_OBJS, $(23_pubsubmix_CXXFLAGS))))

DEPS = $(23_common_OBJS:%.o=%.d)
-include $(DEPS)

$(23_common_TARGET_BIN): $(23_common_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
