# aliases
AREG_OUTPUT_OBJ = $(ProjObjDir)
AREG_OUTPUT_LIB = $(ProjLibDir)
AREG_OUTPUT_BIN = $(ProjBinDir)
AREG_INCLUDES   = $(ProjIncludes)
AREG_TOOLCHAIN  = $(CrossCompile)$(Toolset)
AREG_AR         = $(CrossCompile)ar
AREG_OS         = $(os)

ifeq ($(areg), shared)
AREG_BINARY = shared
else
AREG_BINARY = static
endif

include $(MAKEFILE_CONFIG_DIR)/user.mk

CXXFLAGS := -g -pthread -std=c++11
LDFLAGS  := -lm -lstdc++ -lrt -pthread

#TODO fix the warnings reported by clang first
ifeq ($(Toolset), clang)
CXXFLAGS += -Werror $(UserDefines)
else
CXXFLAGS += -Wall -Werror $(UserDefines)
endif

ifeq ($(Config), Release)
# optimixation with O2?
CXXFLAGS += -O2 -DNDEBUG
else
CXXFLAGS += -DDEBUG
endif

# only native compilers on x86_64 can support bitness?
ifeq ($(Platform), x86_64)
ifeq ($(CrossCompile), )
ifeq ($(bit), 32)
CXXFLAGS += -m32
else ifeq ($(bit), 64)
CXXFLAGS += -m64
endif
endif
endif

ifeq ($(AREG_OS), windows)
CXXFLAGS     += -DWINDOWS
OBJ_EXT      := obj
AREG_BIN_EXT := .exe
AREG_LIB_PREFIX :=
ifeq ($(AREG_BINARY), shared)
	AREG_LIB_EXT := .dll
else
	AREG_LIB_EXT := .lib
endif
else
CXXFLAGS     += -DPOSIX
OBJ_EXT      := o
AREG_BIN_EXT := .out
AREG_LIB_PREFIX := lib
ifeq ($(AREG_BINARY), shared)
	AREG_LIB_EXT := .so
else
	AREG_LIB_EXT := .a
endif
endif

define AREG_HELP_MSG
Usage:
	A placeholder for help message
endef
export AREG_HELP_MSG
help:
	@echo "$$AREG_HELP_MSG"

# Define a new target to build object for a given source file.
# {1} source file, {2}, object directory, {3} object list, {4} extra CXXFLAGS
define obj
$(2)/$(patsubst %.cpp,%.$(OBJ_EXT), $(notdir $(1))): ${1}
	@echo Compiling $$@
	@mkdir -p $(2)
	$(AREG_TOOLCHAIN) -c $(CXXFLAGS) ${4} $(AREG_INCLUDES) $$< -o $$@

# add to list of objects
${3} += $(2)/$(patsubst %.cpp,%.$(OBJ_EXT), $(notdir $(1)))
endef

# clean targets
clean:
	rm -rf $(AregRoot)/$(UserDefOutput)
clean_build:
	rm -rf $(ProjOutputDir)
clean_gen:
	rm -rf $(ProjGendDir)

.PHONY: clean clean_build clean_gen help
