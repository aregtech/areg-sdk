# Compile Options
set(CompileOptShared "Wl,-rpath=${AREG_OUTPUT_BIN} -L ${AREG_OUTPUT_BIN}")
set(CompileOptStatic "-L ${AREG_OUTPUT_LIB} -Wl,-Bstatic -Wl,-Bdynamic")

function(setOpt item)
	# including specific flags to each target.
	if($(AREG_BINARY) MATCHES "shared")
		target_compile_definitions(${item} PRIVATE IMP_AREG_DLL)
		target_compile_options(${item} PRIVATE ${CompileOptShared})
	else()
		target_compile_definitions(${item} PRIVATE IMP_AREG_LIB)
		target_compile_options(${item} PRIVATE ${CompileOptStatic})
	endif()

	# -l options for compiler
	target_link_libraries(${item} ${exampleLDFlags} ${LDFLAGS})
	set_target_properties(${item}
		PROPERTIES
		CXX_STANDARD ${exampleCXXStandard}
		CXX_STANDARD_REQUIRED ON
		)
endfunction(setOpt item)

function(setLibOpt item)
	if($(AREG_BINARY) MATCHES "shared")
		add_library(${item} SHARED ${ARGN})
		set_property(TARGET ${item} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${ProjBinDir})
	else()
		add_library(${item} STATIC ${ARGN})
		set_property(TARGET ${item} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${ProjLibDir})
	endif()

	set_property(TARGET ${item} PROPERTY CXX_STANDARD 17)

endfunction(setLibOpt item)

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

include(${AREG_EXAMPLES}/10_locsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/11_locmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/12_pubsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/13_pubmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/14_pubtraffic/CMakeLists.txt)
include(${AREG_EXAMPLES}/15_pubworker/CMakeLists.txt)


# Iterating over fileList and extract each source file,
# to create separate target and executable.
foreach(item IN LISTS fileList)
	unset(SRC)
	set(itemDir ${AREG_EXAMPLES}/${item})
	set(subProjDir ${itemDir}/src)
	include(${AREG_EXAMPLES}/${item}/CMakeLists.txt)

	# Adding target for each example.
	add_executable(${item} ${SRC})
	setopt(${item})
endforeach()
