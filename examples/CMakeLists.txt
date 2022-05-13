
# Compile Options
set(CompileOptShared "Wl,-rpath=${AREG_OUTPUT_BIN} -L ${AREG_OUTPUT_BIN}")
set(CompileOptStatic "-L ${AREG_OUTPUT_LIB} -Wl,-Bstatic -Wl,-Bdynamic")

# Add examples here in this list
list(APPEND fileList
	01_hello
	02_buffer
	03_file
	04_trace
	05_timer
	06_threads
	07_synch
	08_service
	09_svcmulti
)

# Iterating over fileList and extract each source file,
# to create separate target and executable.
foreach(item IN LISTS fileList)
	unset(SRC)
	set(itemDir ${AREG_EXAMPLES}/${item})
	set(myDir ${itemDir}/src)
	include(${AREG_EXAMPLES}/${item}/CMakeLists.txt)

	# Adding target for each example.
	add_executable(${item} ${SRC})

	# including specific flags to each target.
	if($(AREG_BINARY) MATCHES "shared")
		target_compile_definitions(${item} PRIVATE IMP_AREG_DLL)
		target_compile_options(${item} PRIVATE ${CompileOptShared})
	else()
		target_compile_definitions(${item} PRIVATE IMP_AREG_LIB)
		target_compile_options(${item} PRIVATE ${CompileOptStatic})
	endif()

	# -l options for compiler
	target_link_libraries(${item} areg m  stdc++ rt pthread ${LDFLAGS})
	set_target_properties(${item}
		PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		)
endforeach()
