13_pubsvcmain_PROJECT_NAME := 13_pubsvcmain

13_pubsvcmain_TARGET_BIN := $(13_pubsvcmain_PROJECT_NAME)$(AREG_BIN_EXT)
13_pubsvcmain_OUTPUT_OBJ := $(13_pubmesh_OUTPUT_OBJ)/$(13_pubsvcmain_PROJECT_NAME)

13_pubsvcmain_SRC  :=
13_pubsvcmain_OBJS :=

include $(13_pubmesh_BASE)/pubsvcmain/src/Makefile

# define one target for each source file
$(foreach cpp, $(13_pubsvcmain_SRC), $(eval $(call obj, $(cpp), $(13_pubsvcmain_OUTPUT_OBJ), 13_pubsvcmain_OBJS, $(13_pubmesh_CXXFLAGS))))

DEPS = $(13_pubsvcmain_OBJS:%.o=%.d)
-include $(DEPS)

$(AREG_OUTPUT_BIN)/$(13_pubsvcmain_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(13_generated_TARGET_BIN) $(13_pubsvcmain_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(13_pubmesh_CXXFLAGS) $(13_pubsvcmain_OBJS) $(13_pubmesh_LDFLAGS) -o $@
