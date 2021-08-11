13_shareipcmix_PROJECT_NAME := 13_shareipcmix

13_shareipcmix_BASE := $(ipcmix_BASE)/shareipcmix

13_shareipcmix_TARGET_BIN := $(13_shareipcmix_PROJECT_NAME)$(AREG_BIN_EXT)
13_shareipcmix_OUTPUT_OBJ := $(ipcmix_OUTPUT_OBJ)/$(13_shareipcmix_PROJECT_NAME)

13_shareipcmix_SRC  :=
13_shareipcmix_OBJS :=

include $(13_shareipcmix_BASE)/src/private/Makefile

# define one target for each source file
$(foreach cpp, $(13_shareipcmix_SRC), $(eval $(call obj, $(cpp), $(13_shareipcmix_OUTPUT_OBJ), 13_shareipcmix_OBJS, $(ipcmix_CXXFLAGS))))
