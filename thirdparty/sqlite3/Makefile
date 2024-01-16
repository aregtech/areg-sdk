
# SQLite project library

AREG_SQLITE_LIB     := sqlite

sqlite_BASE         := $(AREG_THIRDPARTY)/sqlite3
sqlite_OUTPUT_OBJ   := $(AREG_OUTPUT_OBJ)/sqlite3
sqlite_TARGET_BIN   := $(AREG_OUTPUT_LIB)/$(AREG_LIB_PREFIX)$(AREG_SQLITE_LIB)$(AREG_STATIC_LIB)
sqlite_CCFLAGS      := -I$(sqlite_BASE)/amalgamation -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_ENABLE_MATH_FUNCTIONS=0 -DSQLITE_ENABLE_WHERETRACE=0


sqlite_SRC	= $(sqlite_BASE)/amalgamation/sqlite3.c
sqlite_OBJ	= $(sqlite_OUTPUT_OBJ)/sqlite3.o

sqlite: $(sqlite_TARGET_BIN) $(sqlite_OBJ)

# Build step for C source
$(sqlite_OBJ): $(sqlite_SRC)
	@echo "Compiling $@ ..."
	@mkdir -p $(dir $@)
	$(AREG_C_COMPILER) $(sqlite_CCFLAGS) -c $(sqlite_SRC) -o $(sqlite_OBJ)

$(sqlite_TARGET_BIN): $(sqlite_OBJ)
	@echo "Linking $@ ..."
	@mkdir -p $(dir $(AREG_OUTPUT_LIB))
	$(AREG_AR) -rcs $(sqlite_OBJ)

.PHONY: sqlite
