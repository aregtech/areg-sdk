# SQLite project library
sqlite_OUTPUT_OBJ   := $(AREG_OUTPUT_OBJ)/sqlite3
sqlite_TARGET_BIN   := $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)sqlite3$(AREG_STATIC_LIB)
sqlite_CCFLAGS      := -I$(sqlite_BASE)/amalgamation -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_ENABLE_MATH_FUNCTIONS=0 -DSQLITE_ENABLE_WHERETRACE=0

sqlite_SRC	= $(sqlite_BASE)/amalgamation/sqlite3.c
sqlite_OBJ	= 

$(foreach src, $(sqlite_SRC), $(eval $(call objcc, $(src), $(sqlite_OUTPUT_OBJ), sqlite_OBJ, $(sqlite_CCFLAGS))))

DEPS = $(sqlite_OBJ:%.o=%.d)
-include $(DEPS)

$(sqlite_TARGET_BIN): $(sqlite_OBJ)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_AR) -rcs $@ $^

sqlite: $(sqlite_TARGET_BIN) $(sqlite_OBJ)
.PHONY: sqlite

