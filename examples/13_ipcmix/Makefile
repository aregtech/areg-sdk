ipcmix_PROJECT_NAME := 13_ipcmix

ipcmix_BASE := $(AREG_EXAMPLES)/13_ipcmix

ipcmix_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/$(ipcmix_PROJECT_NAME)
ipcmix_OUTPUT_LIB := $(AREG_OUTPUT_LIB)
ipcmix_OUTPUT_BIN := $(AREG_OUTPUT_BIN)

ipcmix_CXXFLAGS := -I$(ipcmix_BASE) -DIMP_AREG_LIB

mainipcmix_TARGET_BIN   := mainipcmix$(AREG_BIN_EXT)
clientipcmix_TARGET_BIN := clientipcmix$(AREG_BIN_EXT)
secondipcmix_TARGET_BIN := secondipcmix$(AREG_BIN_EXT)


############# shareipcmix #############
shareipcmix_SRC  :=
shareipcmix_OBJS :=
include $(ipcmix_BASE)/shareipcmix/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(shareipcmix_SRC), $(eval $(call obj, $(cpp), $(ipcmix_OUTPUT_OBJ), shareipcmix_OBJS, $(ipcmix_CXXFLAGS))))


############# mainipcmix #############
mainipcmix_SRC  :=
mainipcmix_OBJS :=
include $(ipcmix_BASE)/mainipcmix/src/Makefile

# needed as main/client/second ipcmix has their own definition of LocalServiceComponent.cpp
mainipcmix_OUTPUT_OBJ := $(ipcmix_OUTPUT_OBJ)/mainipcmix

# define one target for each source file
$(foreach cpp, $(mainipcmix_SRC), $(eval $(call obj, $(cpp), $(mainipcmix_OUTPUT_OBJ), mainipcmix_OBJS, $(ipcmix_CXXFLAGS))))

$(ipcmix_OUTPUT_BIN)/$(mainipcmix_TARGET_BIN): $(areg_OBJS) $(shareipcmix_OBJS) $(mainipcmix_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $^ $(LDFLAGS) -o $@


############# clientipcmix #############
clientipcmix_SRC  :=
clientipcmix_OBJS :=
include $(ipcmix_BASE)/clientipcmix/src/Makefile

# needed as main/client/second ipcmix has their own definition of LocalServiceComponent.cpp
clientipcmix_OUTPUT_OBJ := $(ipcmix_OUTPUT_OBJ)/clientipcmix

# define one target for each source file
$(foreach cpp, $(clientipcmix_SRC), $(eval $(call obj, $(cpp), $(clientipcmix_OUTPUT_OBJ), clientipcmix_OBJS, $(ipcmix_CXXFLAGS))))

$(ipcmix_OUTPUT_BIN)/$(clientipcmix_TARGET_BIN): $(areg_OBJS) $(shareipcmix_OBJS) $(clientipcmix_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $^ $(LDFLAGS) -o $@


############# secondipcmix #############
secondipcmix_SRC  :=
secondipcmix_OBJS :=
include $(ipcmix_BASE)/secondipcmix/src/Makefile

# needed as main/client/second ipcmix has their own definition of LocalServiceComponent.cpp
secondipcmix_OUTPUT_OBJ := $(ipcmix_OUTPUT_OBJ)/secondipcmix

# define one target for each source file
$(foreach cpp, $(secondipcmix_SRC), $(eval $(call obj, $(cpp), $(secondipcmix_OUTPUT_OBJ), secondipcmix_OBJS, $(ipcmix_CXXFLAGS))))

$(ipcmix_OUTPUT_BIN)/$(secondipcmix_TARGET_BIN): $(areg_OBJS) $(shareipcmix_OBJS) $(secondipcmix_OBJS)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_TOOLCHAIN) $^ $(LDFLAGS) -o $@

13_ipcmix: $(ipcmix_OUTPUT_BIN)/$(mainipcmix_TARGET_BIN) $(ipcmix_OUTPUT_BIN)/$(clientipcmix_TARGET_BIN) $(ipcmix_OUTPUT_BIN)/$(secondipcmix_TARGET_BIN)
.PHONY: mainipcmix clientipcmix secondipcmix
