# aliases
AREG_OUTPUT_OBJ = $(ProjObjDir)
AREG_OUTPUT_LIB = $(ProjLibDir)
AREG_OUTPUT_BIN = $(ProjBinDir)
AREG_INCLUDES   = $(ProjIncludes)
AREG_TOOLCHAIN  = $(CrossCompile)$(Toolset)
AREG_AR         = $(CrossCompile)ar
AREG_OS         = $(OpSystem)
AREG_STATIC_LIB =

include $(MAKEFILE_CONFIG_DIR)/user.mk

ifeq ($(areg), static)
AREG_BINARY = static
else
AREG_BINARY = shared
endif

CXXFLAGS := -g -pthread -std=c++17
LDFLAGS  := -lm -lstdc++ -lrt -pthread

#TODO fix the warnings reported by clang first
ifeq ($(findstring clang,$(Toolset)),clang)
CXXFLAGS += -Werror $(UserDefines) -stdlib=libc++
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
AREG_STATIC_LIB := .lib
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
AREG_STATIC_LIB := .a
ifeq ($(AREG_BINARY), shared)
	AREG_LIB_EXT := .so
else
	AREG_LIB_EXT := .a
endif
endif

define AREG_HELP_MSG
Usage: make [target] [areg=<static|shared>] [Config=<Release|Debug>] [Toolset=<toolset>] [CrossCompile=<cross-toolchain>] ...
    target:
        all:            Build all the project. This is the default target.
        framework:      Build areg framework including areg library and mcrouter.
        examples:       Build areg library and examples.
        clean:          Clean build directory
        help:           Show this help message

    options:
        areg:           Areg library type (shared or static). Default is shared.
        Config:         Release or Debug build.
        Toolset:        compiler to use, current supported ones is g++/gcc/clang. Default is g++.
        CrossCompile:   Cross compiler prefix. I.e: use arm-linux-gnueabihf- for arm32
        bit:            bitness of the target binary, supported for x86_64's compilers only for now.

    More options can be set directly in conf/make/user.mk
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
	$(AREG_TOOLCHAIN) -c $(CXXFLAGS) ${4} $(AREG_INCLUDES) -MMD $$< -o $$@

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
