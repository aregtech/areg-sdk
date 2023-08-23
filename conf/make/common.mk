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
AREG_AR                 := ar
AREG_TOOLCHAIN          = $(CXX)


ifeq ($(AREG_BUILD_TYPE), Release)
    AREG_DEFINITIONS    += -DNDEBUG
else
    AREG_DEFINITIONS    += -DDEBUG
endif

ifeq ($(AREG_COMPILER_ID), Clang)

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
    AREG_LDFLAGS            += -lm -lstdc++ -lrt -pthread

    OBJ_EXT         := o
    AREG_BIN_EXT    := .out
    AREG_STATIC_LIB := .a
    AREG_LIB_PREFIX := lib
    ifeq ($(AREG_BINARY), shared)
    	AREG_LIB_EXT := .so
    else
    	AREG_LIB_EXT := .a
    endif

else ifeq ($(AREG_COMPILER_ID), GNU)

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
    AREG_LDFLAGS            += -lm -lstdc++ -lrt -pthread

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
    AREG_LDFLAGS            += -lstdc++ -lm -lpthread -lrt

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
AREG_PRODUCT_PATH   := $(shell echo build/$(AREG_COMPILER_FAMILY)-$(AREG_TOOLCHAIN)/$(AREG_OS)-$(AREG_BITNESS)-$(AREG_PLATFORM)-$(AREG_BUILD_TYPE) | tr '[:upper:]' '[:lower:]')

# If 'AREG_OUTPUT_DIR' is not set, build and set the default path.
AREG_OUTPUT_DIR := $(if $(AREG_OUTPUT_DIR),$(AREG_OUTPUT_DIR),$(AREG_BUILD_ROOT)/$(AREG_PRODUCT_PATH))

# If 'AREG_OUTPUT_LIB' is not set, build and set the default path.
AREG_OUTPUT_LIB := $(if $(AREG_OUTPUT_LIB),$(AREG_OUTPUT_LIB),$(AREG_OUTPUT_DIR)/lib)

# If 'AREG_OUTPUT_BIN' is not set, build and set the default path.
AREG_OUTPUT_BIN := $(if $(AREG_OUTPUT_BIN),$(AREG_OUTPUT_BIN),$(AREG_OUTPUT_DIR)/bin)

# The absolute path for compiled object files.
AREG_OUTPUT_OBJ := $(AREG_OUTPUT_DIR)/obj

# The project include directories
AREG_INCLUDES   := -I$(AREG_BASE) -I$(AREG_GENERATE_DIR)

ifeq ($(AREG_BINARY),shared)
    AREG_LIB_INCLUDES = -L $(AREG_OUTPUT_BIN) -L $(AREG_OUTPUT_LIB)
else
    AREG_LIB_INCLUDES = -L $(AREG_OUTPUT_LIB)
endif

AREG_EXTENDED_LIBS    :=
ifneq ($(AREG_EXTENDED),0)
$(info Compile AREG extended library with enabled extensions)
	AREG_EXTENDED_LIBS    := -lncurses
    ifneq ($(AREG_OS), Cygwin)
		AREG_EXTENDED_LIBS+= -ltinfo
    endif
endif

# set compiler flags
CXXFLAGS    += $(AREG_COMPILER_OPTIONS)
# set linker flags, include AREG_OUTPUT_BIN to the library search path
LDFLAGS     += $(AREG_LDFLAGS) -Wl,-R$(AREG_OUTPUT_BIN)

$(info -------------------- Makefile Status Report Begin --------------------)
$(info >>> Build for '$(AREG_OS)' '$(AREG_BITNESS)'-bit platform '$(AREG_PLATFORM)' with compiler '$(AREG_CXX_COMPILER)', ID '$(AREG_COMPILER_FAMILY)', and build type '$(AREG_BUILD_TYPE)')
$(info >>> Binary output folder '$(AREG_OUTPUT_BIN)', executable extensions '$(AREG_BIN_EXT)')
$(info >>> Generated files are in '$(AREG_GENERATE_DIR)', library output folder '$(AREG_OUTPUT_LIB)')
$(info >>> Build AREG Extensions are '$(AREG_EXTENDED)', compile with logs '$(AREG_LOGS)')
$(info "-------------------- Makefile Status Report End ----------------------")

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
	rm -rf $(AREG_BUILD_ROOT)
clean_build:
	rm -rf $(AREG_OUTPUT_DIR)
clean_gen:
	rm -rf $(AREG_GENERATE_DIR)

.PHONY: clean clean_build clean_gen help
