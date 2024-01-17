
gtest_main_BASE			:= $(googletest_BASE)/googletest
gtest_main_OUTPUT_OBJ	:= $(AREG_OUTPUT_OBJ)/gtest_main
gtest_main_TARGET_BIN	:= $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)gtest_main$(AREG_STATIC_LIB)
gtest_main_CXXFLAGS		:= -I$(gtest_main_BASE) -I$(gtest_main_BASE)/include -I$(gtest_main_BASE)/src -I$(googletest_BASE)

gtest_main_SRC			= $(gtest_main_BASE)/src/gtest-all.cc $(gtest_main_BASE)/src/gtest_main.cc
gtest_main_OBJS			= 

# define one target for each source file
$(foreach src, $(gtest_main_SRC), $(eval $(call objccp, $(src), $(gtest_main_OUTPUT_OBJ), gtest_main_OBJS, $(gtest_main_CXXFLAGS))))

DEPS = $(gtest_main_OBJS:%.o=%.d)
-include $(DEPS)

$(gtest_main_TARGET_BIN): $(gtest_main_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^

gtest_main: $(gtest_main_TARGET_BIN) $(gtest_main_OBJS)
.PHONY: gtest_main
