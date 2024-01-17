
sqlite_BASE     := $(AREG_THIRDPARTY)/sqlite3
googletest_BASE	:= $(AREG_THIRDPARTY)/googletest

ifneq ($(AREG_BUILD_TESTS),0)

include $(AREG_THIRDPARTY)/sqlite3/Makefile
include $(AREG_THIRDPARTY)/gtest.mk
include $(AREG_THIRDPARTY)/gtest_main.mk

thirdparty: sqlite gtest gtest_main
.PHONY: thirdparty

else

include $(AREG_THIRDPARTY)/sqlite3/Makefile

thirdparty: sqlite
.PHONY: thirdparty

endif
