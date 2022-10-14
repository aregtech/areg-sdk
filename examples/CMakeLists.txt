# This function will be used for all targets which created by add_executable(),
# to set proper compile options and linkage flags
function(setOpt item)

	if(AREG_BINARY MATCHES shared)
		target_compile_definitions(${item} PRIVATE IMP_AREG_DLL)
		target_compile_options(${item} PRIVATE ${ExampleCompileOptShared})
	else()
		target_compile_definitions(${item} PRIVATE IMP_AREG_LIB)
		target_compile_options(${item} PRIVATE ${ExampleCompileOptStatic})
	endif()

	# Linking flags
	target_link_libraries(${item} ${exampleLDFlags})

	# Adjusting CPP standard for target
	set_target_properties(${item}
		PROPERTIES
		CXX_STANDARD ${aregCXXStandard}
		CXX_STANDARD_REQUIRED ON
		)
endfunction(setOpt item)

function(setLibOpt item)
	add_library(${item} STATIC ${ARGN})

	# Setting libxxx properties (static or shared)
	if(AREG_BINARY MATCHES shared)
		target_compile_definitions(${item} PRIVATE IMP_AREG_DLL)
		target_compile_options(${item} PRIVATE ${ExampleCompileOptShared})

		set_property(TARGET ${item} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${ProjBinDir})
	else()
		target_compile_definitions(${item} PRIVATE IMP_AREG_LIB)
		target_compile_options(${item} PRIVATE ${ExampleCompileOptStatic})

		set_property(TARGET ${item} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${ProjLibDir})
	endif()

	# Linking flags
	target_link_libraries(${item} PRIVATE ${exampleLDFlags})

	# Adjusting CPP standard for target
	set_target_properties(${item}
		PROPERTIES
		CXX_STANDARD ${aregCXXStandard}
		CXX_STANDARD_REQUIRED ON
		)

endfunction(setLibOpt item)

# Examples which has similar building structure
list(APPEND projectList
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

# Examples with specific design must be included one by one.
include(${AREG_EXAMPLES}/00_helloservice/CMakeLists.txt)
include(${AREG_EXAMPLES}/10_locsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/11_locmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/12_pubsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/13_pubmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/14_pubtraffic/CMakeLists.txt)
include(${AREG_EXAMPLES}/15_pubworker/CMakeLists.txt)
include(${AREG_EXAMPLES}/16_pubfsm/CMakeLists.txt)
include(${AREG_EXAMPLES}/18_locwatchdog/CMakeLists.txt)
include(${AREG_EXAMPLES}/19_pubwatchdog/CMakeLists.txt)
include(${AREG_EXAMPLES}/20_largedata/CMakeLists.txt)


# Iterating over projectList and extract each source file,
# to create separate target and executable.
foreach(item IN LISTS projectList)
	unset(SRC)
	set(itemDir ${AREG_EXAMPLES}/${item})
	set(subProjDir ${itemDir}/src)
	include(${AREG_EXAMPLES}/${item}/CMakeLists.txt)

	# Adding target for each example.
	add_executable(${item} ${SRC})
	setopt(${item})
endforeach()
