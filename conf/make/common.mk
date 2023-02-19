# ###########################################################################
#               PROJECT VARIABLES
# Change these variables only if indeed there is a need.
# ###########################################################################

AREG_DEVELOP_ENV        :=
AREG_LDFLAGS            :=
AREG_COMPILER_OPTIONS   :=
AREG_INCLUDES           :=
AREG_LIB_INCLUDES       :=
AREG_DEFINITIONS        := $(AREG_USER_DEFINES) -DUNICODE -D_UNICODE

ifeq ($(AREG_BUILD_TYPE), Release)
    AREG_DEFINITIONS    += -DNDEBUG
else
    AREG_DEFINITIONS    += -DDEBUG
endif

ifeq ($(AREG_CXX_COMPILER_ID), Clang)

    AREG_DEFINITIONS        += -DPOSIX
    AREG_DEVELOP_ENV        := Posix

    ifeq ($(AREG_BUILD_TYPE), Release)
        AREG_COMPILER_OPTIONS   += -O2
    else
        AREG_COMPILER_OPTIONS  += -O0 -g3
    endif

    ifeq ($(AREG_BITNESS), 32)
        AREG_COMPILER_OPTIONS   += -m32
    else
        AREG_COMPILER_OPTIONS   += -m64
    endif

    AREG_COMPILER_OPTIONS   += -g -pthread -std=c++17 -Werror -Wall -fmessage-length=0 $(AREG_DEFINITIONS) -stdlib=libstdc++
    AREG_LDFLAGS            += -lm -lstdc++ -lrt -pthread $(AREG_USER_DEF_LIBS)

    OBJ_EXT         := o
    AREG_BIN_EXT    := .out
    AREG_STATIC_LIB := .a
    AREG_LIB_PREFIX := lib
    ifeq ($(AREG_BINARY), shared)
    	AREG_LIB_EXT := .so
    else
    	AREG_LIB_EXT := .a
    endif

else ifeq ($(AREG_CXX_COMPILER_ID), GNU)

    AREG_DEFINITIONS        += -DPOSIX
    AREG_DEVELOP_ENV        := Posix
    AREG_COMPILER_OPTIONS   := -g -pthread -Wall -Werror -fmessage-length=0 -fPIC

    ifeq ($(AREG_BUILD_TYPE), Release)
        AREG_COMPILER_OPTIONS   += -O2
    else
        AREG_COMPILER_OPTIONS   += -O0 -g3
    endif

    ifeq ($(AREG_BITNESS), 32)
        AREG_COMPILER_OPTIONS   += -m32
    else
        AREG_COMPILER_OPTIONS   += -m64
    endif

    AREG_COMPILER_OPTIONS   += $(AREG_DEFINITIONS)
    AREG_LDFLAGS            += -lm -lstdc++ -lrt -pthread $(AREG_USER_DEF_LIBS)

    ifeq ($(AREG_OS), Cygwin)

        AREG_COMPILER_OPTIONS   += -std=gnu++17
        OBJ_EXT         := o
        AREG_BIN_EXT    := .exe
        AREG_STATIC_LIB := .lib
        AREG_LIB_PREFIX :=
        ifeq ($(AREG_BINARY), shared)
    	    AREG_LIB_EXT := .dll
        else
    	    AREG_LIB_EXT := .lib
        endif

    else
        AREG_COMPILER_OPTIONS   += -std=c++17

        OBJ_EXT         := o
        AREG_BIN_EXT    := .out
        AREG_STATIC_LIB := .a
        AREG_LIB_PREFIX := lib
        ifeq ($(AREG_BINARY), shared)
    	    AREG_LIB_EXT := .so
        else
    	    AREG_LIB_EXT := .a
        endif

    endif

else

    $(warning >>> Unsupported compiler type. The result is unpredictable, by default use GNU compiler settings and POSIX API)
    AREG_DEFINITIONS    += -DPOSIX
    AREG_DEVELOP_ENV    := Posix

    ifeq ($(AREG_BUILD_TYPE), Release)
        AREG_COMPILER_OPTIONS   += -O2
    else
        AREG_COMPILER_OPTIONS   += -O0 -g3
    endif

    ifeq ($(AREG_BITNESS MATCHES), 32)
        AREG_COMPILER_OPTIONS   += -m32
    else
        AREG_COMPILER_OPTIONS   += -m64
    endif

    AREG_COMPILER_OPTIONS   += -pthread -Wall -fmessage-length=0 -std=c++17
    AREG_LDFLAGS            += -lstdc++ -lm -lpthread -lrt $(AREG_USER_DEF_LIBS)

    OBJ_EXT         := o
    AREG_BIN_EXT    := .out
    AREG_STATIC_LIB := .a
    AREG_LIB_PREFIX := lib
    ifeq ($(AREG_BINARY), shared)
    	AREG_LIB_EXT := .so
    else
    	AREG_LIB_EXT := .a
    endif

endif

# The source code build relative path
ProjBuildPath   := $(shell echo build/$(AREG_CXX_COMPILER)/$(AREG_OS)-$(AREG_PLATFORM)-$(AREG_BUILD_TYPE) | tr '[:upper:]' '[:lower:]')

# The project output directory
ProjOutputDir   := $(AREG_BUILD_ROOT)/$(AREG_USER_DEF_OUTPUT_DIR)/$(ProjBuildPath)
# The project generated files directory
ProjGendDir     := $(AREG_BUILD_ROOT)/$(AREG_USER_DEF_OUTPUT_DIR)/generate
# The common object directory, projects can have their own sub-directories
ProjObjDir      := $(ProjOutputDir)/obj

# The project static library directory
ProjLibDir      := $(ProjOutputDir)/lib
# The project binary output directory
ProjBinDir      := $(ProjOutputDir)/bin

# The project include directories
ProjIncludes    := 
ProjIncludes    += -I$(AREG_BASE)
ProjIncludes    += -I$(ProjGendDir)
ProjIncludes    += -I$(AREG_USER_DEF_INCLUDES)

# aliases
AREG_OUTPUT_OBJ = $(ProjObjDir)
AREG_OUTPUT_LIB = $(ProjLibDir)
AREG_OUTPUT_BIN = $(ProjBinDir)
AREG_INCLUDES   = $(ProjIncludes)
AREG_AR         = ar
AREG_TOOLCHAIN  = $(CXX)

ifeq ($(AREG_BINARY),shared)
    AREG_LIB_INCLUDES = -L $(AREG_OUTPUT_BIN) -L $(AREG_OUTPUT_LIB)
else
    AREG_LIB_INCLUDES = -L $(AREG_OUTPUT_LIB)
endif

AREG_EXTENSIONS_LIBS    :=
ifneq ($(AREG_ENABLE_EXT),0)
$(info Compile AREG with enabled extensions)

	AREG_COMPILER_OPTIONS   += -DENABLE_AREG_EXTENSIONS
	AREG_EXTENSIONS_LIBS    := -lncurses
    ifneq ($(AREG_OS), Cygwin)
        AREG_EXTENSIONS_LIBS+= -ltinfo
    endif
endif

CXXFLAGS    += $(AREG_COMPILER_OPTIONS)
LDFLAGS     += $(AREG_LDFLAGS)

$(info Builds applications in the folder $(AREG_OUTPUT_BIN))
$(info Generated files are in $(ProjGendDir))
$(info Builds $(AREG_BINARY) communication engine)

define AREG_HELP_MSG
# Usage: make [target] [AREG_BINARY=<static|shared>] [AREG_BUILD_TYPE=<Release|Debug>] [AREG_CXX_COMPILER=<compiler>] [CrossCompile=<cross-toolchain>] ...
Usage: make [target] [AREG_BINARY=<static|shared>] [AREG_BUILD_TYPE=<Release|Debug>] [AREG_CXX_COMPILER=<compiler>]
    target:
        all:            Build all the project. This is the default target.
        framework:      Build framework, which including areg library and mcrouter.
        examples:       Build areg library and examples.
        clean:          Clean build directory
        help:           Show this help message

    options:
        AREG_BINARY:        areg library type (shared or static). Default is shared.
        AREG_BUILD_TYPE:    Release or Debug build.
        AREG_CXX_COMPILER:  compiler to use, current supported ones is g++/gcc/clang. Default is g++.
        AREG_BITNESS:       bitness of the target binary, supported for x86_64's compilers only for now.

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
	rm -rf $(AREG_BUILD_ROOT)/$(AREG_USER_DEF_OUTPUT_DIR)
clean_build:
	rm -rf $(ProjOutputDir)
clean_gen:
	rm -rf $(ProjGendDir)

.PHONY: clean clean_build clean_gen help
