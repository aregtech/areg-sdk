# ###########################################################################
# This file is part of AREG SDK
# Copyright (c) 2017-2022, Aregtech UG
# Contact: info[at]aregtech.com
# ###########################################################################

# ###########################################################################
# These parameters already should be set either from command line or by extracting from other params
# 
# MakefileRoot  	-- indicates the path of the root Makefile, should be extracted from $(abspath $(lastword $(MAKEFILE_LIST)))
# AREG_CXX_COMPILER	-- gcc, g++, clang++13, etc., the default is g++
# AREG_BUILD_TYPE	-- 'Debug' or 'Release'. Parameters are either passed in the command line or extracted. The default is 'Release'
# AREG_PLATFORM		-- x86, x86_64, arm, etc. Parameters are either passed in the command line or extracted. The default is current system
# AREG_OS      		-- Windows, Linux, Darvin, etc. Parameters are either passed in the command line, or extracted. The default is current system
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

AREG_CXX_COMPILER_ID:=
AREG_CXX_COMPILER   :=

ifneq ($(AREG_COMPILER_FAMILY),)
$(info Selected compiler family '$(AREG_COMPILER_FAMILY)')

	ifeq ($(AREG_COMPILER_FAMILY), gnu)
		AREG_CXX_COMPILER_ID:= GNU
		AREG_CXX_COMPILER	:= g++
	else ifeq ($(AREG_COMPILER_FAMILY), cygwin)
		AREG_CXX_COMPILER_ID:= GNU
		AREG_CXX_COMPILER	:= g++
	else ifeq ($(AREG_COMPILER_FAMILY), clang)
		AREG_CXX_COMPILER_ID:= Clang
		AREG_CXX_COMPILER	:= clang++
	else
		AREG_CXX_COMPILER_ID:= GNU
		AREG_CXX_COMPILER	:= g++
	endif

else ifneq ($(AREG_CXX_COMPILER),)

	ifeq ($(AREG_CXX_COMPILER),g++)
		AREG_CXX_COMPILER_ID:= GNU
	else ifeq ($(AREG_CXX_COMPILER),gcc)
		AREG_CXX_COMPILER_ID:= GNU
	else ifeq ($(AREG_CXX_COMPILER),clang)
		AREG_CXX_COMPILER_ID:= Clang
	else ifeq ($(AREG_CXX_COMPILER),clang++)
		AREG_CXX_COMPILER_ID:= Clang
	endif

else
	# Modify the 'AREG_CXX_COMPILER' to change compiler
	
	AREG_CXX_COMPILER	 = g++
	AREG_CXX_COMPILER_ID = GNU
    
	# AREG_CXX_COMPILER	 = clang++
	# AREG_COMPILER_FAMILY:= Clang

endif


# ###########################################################################
# Settings
# ###########################################################################

# Target settings. Modify AREG_BINARY to compile target areg framewor library
# either as a "static" or a "shared" library
AREG_BINARY := static
# AREG_BINARY := shared

# Modify the 'AREG_BUILD_TYPE' to change the "Release" or "Debug" build configuration
AREG_BUILD_TYPE := Debug
# AREG_BUILD_TYPE := Release

AREG_OUTPUT_DIR	=
AREG_OS 		= $(DETECTED_OS)
AREG_PLATFORM 	= $(DETECTED_PROCESSOR)
AREG_BITNESS	= $(DETECTED_BITNESS)

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
# The 'ENABLE_TRACES' enables logs in the binaries.
AREG_USER_DEFINES		:= -DENABLE_TRACES

# User can set specific include paths, must be prefixed with '-I' if used
AREG_USER_DEF_INCLUDES	:= 

# User can set the specific library paths to search libraries
AREG_USER_DEF_LIB_PATHS	:= 

# User can set specific libraries to link.
AREG_USER_DEF_LIBS     	:= 

# User output folder
AREG_USER_DEF_OUTPUT_DIR:= product
