# ###########################################################################
# This file is part of AREG SDK
# Copyright (c) 2017-2022, Aregtech UG
# Contact: info[at]aregtech.com
# ###########################################################################

# ###########################################################################
# These parameters already should be set either from command line or by extracting from other params
# 
# MakefileRoot  		-- indicates the path of the root Makefile, should be extracted from $(abspath $(lastword $(MAKEFILE_LIST)))
# AREG_COMPILER_FAMILY	-- simple way to set CXX and CC compilers. Possible values: gnu for g++/gcc, llvm for clang++/clang and cygwin to compile with GNU g++/gcc compilers under Windows.
# AREG_COMPILER			-- Set the CXX and CC compilers. Possible values: gcc, g++, clang++ and clang. If C++ compiler is set, it will guess the CC compiler. The defaults are g++ / gcc compilers
# AREG_OS      			-- 'Windows' or 'Linux' OS. The default is current system.
# AREG_PLATFORM			-- Sets the CPU platform. Possible values: x86, x86_64, arm and aarch64. The default is current system
# AREG_BITNESS			-- Bitness of binaries. Possible values: 32 or 64. The default is current system
# AREG_BINARY			-- The binary type of AREG library. Possible values: 'shared' or 'static'. The default is 'shared'.
# AREG_BUILD_TYPE		-- 'Debug' or 'Release' build types of codes. The default is 'Release'. The default is 'Release'
# AREG_EXTENDED			-- Enabled or disable AREG extensions, which may create additional library dependencies. By default is OFF.
# AREG_LOGS				-- Enable or disable compilation with logs.
#
# The following is a list of preprocessor directives, depending on the settings above:
#   1. -DDEBUG or -DNDEBUG, depending whether AREG_BUILD_TYPE name is "Debug" or not, default is -DNDEBUG ("Release")
#   2. -DBIT64 or -DBIT32, depending whether AREG_PLATFORM is x64 or not, default is -DBIT32
#   3. -DWINDOWS or -DPOSIX, depending whether OpSystesm is Windows or not, the default is -DPOSIX
# 
# ###########################################################################


# ###########################################################################
# Toolchain
# ###########################################################################

AREG_COMPILER_ID	:=
AREG_CXX_COMPILER   :=
AREG_C_COMPILER		:=

AREG_COMPILER_FAMILY:= $(if $(AREG_COMPILER_FAMILY),$(AREG_COMPILER_FAMILY),)
AREG_COMPILER		:= $(if $(AREG_COMPILER),$(AREG_COMPILER),)
AREG_OS 			:= $(if $(AREG_OS),$(AREG_OS),$(DETECTED_OS))
AREG_PLATFORM		:= $(if $(AREG_PLATFORM),$(AREG_PLATFORM),$(DETECTED_PROCESSOR))
AREG_BITNESS		:= $(if $(AREG_BITNESS),$(AREG_BITNESS),$(DETECTED_BITNESS))

ifneq ($(strip $(AREG_COMPILER_FAMILY)),)
$(info Selected compiler family '$(AREG_COMPILER_FAMILY)')

	ifeq ($(AREG_COMPILER_FAMILY), gnu)
		AREG_COMPILER_ID	:= GNU
		AREG_CXX_COMPILER	:= g++
		AREG_C_COMPILER		:= gcc
	else ifeq ($(AREG_COMPILER_FAMILY), cygwin)
		AREG_COMPILER_ID	:= GNU
		AREG_CXX_COMPILER	:= g++
		AREG_C_COMPILER		:= gcc
	else ifeq ($(AREG_COMPILER_FAMILY), llvm)
		AREG_COMPILER_ID	:= Clang
		AREG_CXX_COMPILER	:= clang++
		AREG_C_COMPILER		:= clang
	else
		AREG_COMPILER_ID	:= GNU
		AREG_CXX_COMPILER	:= g++
		AREG_C_COMPILER		:= gcc
	endif

else ifneq ($(strip $(AREG_COMPILER)),)

	ifeq ($(AREG_COMPILER),g++)
		AREG_COMPILER_ID	:= GNU
		AREG_CXX_COMPILER	:= $(AREG_COMPILER)
		AREG_C_COMPILER		:= gcc
		AREG_COMPILER_FAMILY:= gnu
		ifeq ($(DETECTED_OS),Cygwin)
			AREG_COMPILER_FAMILY:= cygwin
		endif
	else ifeq ($(AREG_COMPILER),gcc)
		AREG_COMPILER_ID	:= GNU
		AREG_CXX_COMPILER	:= $(AREG_COMPILER)
		AREG_C_COMPILER		:= gcc
		AREG_COMPILER_FAMILY:= gnu
		ifeq ($(DETECTED_OS),Cygwin)
			AREG_COMPILER_FAMILY:= cygwin
		endif
	else ifeq ($(AREG_COMPILER),clang++)
		AREG_COMPILER_ID	:= Clang
		AREG_CXX_COMPILER	:= $(AREG_COMPILER)
		AREG_C_COMPILER		:= clang
		AREG_COMPILER_FAMILY:= llvm
	else ifeq ($(AREG_COMPILER),clang)
		AREG_COMPILER_ID	:= Clang
		AREG_CXX_COMPILER	:= $(AREG_COMPILER)
		AREG_C_COMPILER		:= clang
		AREG_COMPILER_FAMILY:= llvm
	else
		AREG_COMPILER_ID	:= Unknown
		AREG_CXX_COMPILER	:= $(AREG_COMPILER)
		AREG_C_COMPILER		:= $(AREG_COMPILER)
		AREG_COMPILER_FAMILY:= Unknown
	endif

else
	# Modify the 'AREG_CXX_COMPILER' to change compiler
	
	AREG_CXX_COMPILER	:= g++
	AREG_C_COMPILER		:= gcc
	AREG_COMPILER_ID	:= GNU
	AREG_COMPILER_FAMILY:= gnu

	# AREG_CXX_COMPILER	:= clang++
	# AREG_C_COMPILER	:= clang
	# AREG_COMPILER_FAMILY:= llvm

endif

CXX := $(AREG_CXX_COMPILER)
CC	:= $(AREG_C_COMPILER)
$(info >>> Selected compilers: CXX = $(CXX), CC = $(CC))

# ###########################################################################
# Settings
# ###########################################################################

# Target settings. Modify AREG_BINARY to compile target areg framework library
# either as a "static" or a "shared" library
# AREG_BINARY := static
AREG_BINARY := $(if $(AREG_BINARY),$(AREG_BINARY),shared)

# Modify the 'AREG_BUILD_TYPE' to change the "Release" or "Debug" build configuration
# AREG_BUILD_TYPE := Debug
AREG_BUILD_TYPE := $(if $(AREG_BUILD_TYPE),$(AREG_BUILD_TYPE),Release)

# Compile AREG extended features if set 1. By default, it is 0.
# Otherwise, disable extensions by setting 1
AREG_EXTENDED := $(if $(AREG_EXTENDED),$(AREG_EXTENDED),0)

# Modify 'AREG_LOGS' to enable or disable compilation with logs.
# By default, the applications are compiled with logs.
AREG_LOGS := $(if $(AREG_LOGS),$(AREG_LOGS),1)

# ###########################################################################
#           AERG SDK globals
# These AREG SDK variables are fixed and should not be changed
# ###########################################################################

# AREG SDK root directory without '/'
AREG_BUILD_ROOT 	:= $(AREG_SDK_ROOT)

# ###########################################################################
#           USER DEFINED VARIABLES
# These are user specific settings to apply to all projects.
# The user must be able to change the target output folder
# ###########################################################################

# User can set specific preprocessor define symbols.
AREG_USER_DEFINES	:= -DAREG_LOGS=$(AREG_LOGS) -DAREG_EXTENDED=$(AREG_EXTENDED)

# User output folder
AREG_USER_PRODUCTS	:= product
