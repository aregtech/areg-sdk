
gtest_BASE				:= $(googletest_BASE)/googletest
gtest_OUTPUT_OBJ		:= $(AREG_OUTPUT_OBJ)/gtest
gtest_TARGET_BIN		:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)gtest$(AREG_STATIC_LIB)
gtest_CXXFLAGS			:= -I$(gtest_BASE) -I$(gtest_BASE)/include -I$(gtest_BASE)/src -I$(googletest_BASE)

gtest_SRC				= $(gtest_BASE)/src/gtest-all.cc
gtest_OBJS				= 

# define one target for each source file
$(foreach src, $(gtest_SRC), $(eval $(call objccp, $(src), $(gtest_OUTPUT_OBJ), gtest_OBJS, $(gtest_CXXFLAGS))))

DEPS = $(gtest_OBJS:%.o=%.d)
-include $(DEPS)

$(gtest_TARGET_BIN): $(gtest_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^

gtest: $(gtest_TARGET_BIN) $(gtest_OBJS)
.PHONY: gtest
