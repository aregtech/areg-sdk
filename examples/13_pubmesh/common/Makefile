13_common_PROJECT_NAME := 13_common

13_common_TARGET_BIN := $(AREG_LIB_PREFIX)$(13_common_PROJECT_NAME)$(AREG_STATIC_LIB)
13_common_OUTPUT_OBJ := $(13_pubmesh_OUTPUT_OBJ)/common

13_common_SRC  :=
13_common_OBJS :=

include $(13_pubmesh_BASE)/common/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_common_SRC), $(eval $(call obj, $(cpp), $(13_common_OUTPUT_OBJ), 13_common_OBJS, $(13_pubmesh_CXXFLAGS))))

DEPS = $(13_common_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_LIB)/$(13_common_TARGET_BIN): $(13_common_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^
