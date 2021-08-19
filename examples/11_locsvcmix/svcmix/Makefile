11_svcmix_PROJECT_NAME := 11_svcmix

11_svcmix_BASE := $(11_locsvcmix_BASE)/svcmix

11_svcmix_TARGET_BIN := $(11_svcmix_PROJECT_NAME)$(AREG_BIN_EXT)
11_svcmix_OUTPUT_OBJ := $(11_locsvcmix_OUTPUT_OBJ)/$(11_svcmix_PROJECT_NAME)

11_svcmix_SRC  :=
11_svcmix_OBJS :=

include $(11_svcmix_BASE)/src/Makefile

# define one target for each source file
$(foreach cpp, $(11_svcmix_SRC), $(eval $(call obj, $(cpp), $(11_svcmix_OUTPUT_OBJ), 11_svcmix_OBJS, $(11_locsvcmix_CXXFLAGS))))

$(AREG_OUTPUT_BIN)/$(11_svcmix_TARGET_BIN): $(areg_TARGET_PATH) $(AREG_OUTPUT_LIB)/$(11_generated_TARGET_BIN) $(11_svcmix_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $(CXXFLAGS) $(11_locsvcmix_CXXFLAGS) $(11_svcmix_OBJS) $(11_locsvcmix_LDFLAGS) -o $@
