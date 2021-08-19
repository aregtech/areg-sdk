# ###########################################################################
# This file is part of AREG IoT SDK
# Copyright (c) Aregtech UG, 2017-2021
# Contact: info[at]aregtech.com
# ###########################################################################

# ###########################################################################
# These parameters already should be set either from command line or by extracting from other params
# 
# MakefileRoot  -- indicates the path of the root Makefile, should be extracted from $(abspath $(lastword $(MAKEFILE_LIST)))
# Toolset       -- gcc, g++, etc., the default is g++
# Config        -- 'Debug' or 'Release'. Parameters are either passed in the command line or extracted. The default is 'Release'
# Platform      -- x86, x64, arm, etc. Parameters are either passed in the command line or extracted. The default is current system
# OpSystem      -- Windows, Linux, Darvin, etc. Parameters are either passed in the command line, or extracted. The default is current system
#
# The following is a list of preprocessor directives, depending on the settings above:
#   1. -DDEBUG or -DNDEBUG, depending whether Config name is "Debug" or not, default is -DNDEBUG ("Release")
#   2. -DBIT64 or -DBIT32, depending whether Platform is x64 or not, default is -DBIT32
#   3. -DWINDOWS or -DPOSIX, depending whether OpSystesm is Windows or not, the default is -DPOSIX
# 
# ###########################################################################


# ###########################################################################
# Toolchain
# ###########################################################################

Toolset      := g++
CrossCompile :=

# ###########################################################################
# Settings
# ###########################################################################

# taget settings
areg   := static
bit    :=
Config := Release
# Config := Debug

# host settings, can leave them blank in case host's OS is Linux.
Platform :=
OpSystem :=

ifeq ($(Platform),)
Platform := $(shell uname -p)
endif
ifeq ($(OpSystem),)
OpSystem := $(shell uname -s)
endif

# ###########################################################################
#           AERG SDK globals
# These AREG SDK variables are fixed and should not be changed
# ###########################################################################

# AREG SDK root directory without '/'
AregRoot        := $(AREG_SDK_ROOT)

# The source codes of AREG SDK
AregInclude     := $(AREG_BASE)


# ###########################################################################
#           USER DEFINED VARIABLES
# These are user specific settings to apply to all projects.
# The user must be able to change the target output folder
# ###########################################################################

# User can set specific preprocessor directives
UserDefines     := -DENABLE_TRACES

# User can set specific include paths, must be prefixed with '-I' if used
UserDefIncludes := 

# User can set the specific library paths to search libraries
UserDefLibPaths := 

# User can set specific libraries to link.
UserDefLibs     := 

# User output folder
UserDefOutput   := product

# ###########################################################################
#               PROJECT VARIABLES
# Change these variables only if indeed there is a need.
# ###########################################################################

# The source code build relative path
ProjBuildPath   := build/$(CrossCompile)$(Toolset)/$(OpSystem)-$(Platform)-$(Config)

# The project output directory
ProjOutputDir   := $(AregRoot)/$(UserDefOutput)/$(ProjBuildPath)
# The project generated files directory
ProjGendDir     := $(AregRoot)/$(UserDefOutput)/generate
# The common object directory, projects can have their own sub-directories
ProjObjDir      := $(ProjOutputDir)/obj

# The project static library directory
ProjLibDir      := $(ProjOutputDir)/lib
# The project binary output directory
ProjBinDir      := $(ProjOutputDir)/bin

# The project include directories
ProjIncludes    := 
ProjIncludes    += -I$(AregInclude)
ProjIncludes    += -I$(ProjGendDir)
ProjIncludes    += $(UserDefIncludes)
