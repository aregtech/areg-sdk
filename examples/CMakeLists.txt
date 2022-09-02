# This function will be used for all targets which created by add_executable(),
# to set proper compile options and linkage flags
function(setOpt item)

	# Adding compile options to target in respect to ${AREG_BINARY}
	if(AREG_BINARY MATCHES shared)
		target_compile_definitions(${item} PRIVATE IMP_AREG_DLL=1)
		target_compile_options(${item} PRIVATE ${CompileOptShared})
	else()
		target_compile_definitions(${item} PRIVATE IMP_AREG_LIB=1)
		target_compile_options(${item} PRIVATE ${CompileOptStatic})
	endif()

	# Linking flags
	target_link_libraries(${item} ${exampleLDFlags})

	# Adjusting CPP standard for target
	set_target_properties(${item}
		PROPERTIES
		CXX_STANDARD ${exampleCXXStandard}
		CXX_STANDARD_REQUIRED ON
		)
endfunction(setOpt item)

function(setLibOpt item)

	# Setting libxxx properties (static or shared)
	if(AREG_BINARY MATCHES shared)
		add_library(${item} SHARED ${ARGN})
		target_compile_definitions(${item} PRIVATE IMP_AREG_DLL=1)
		target_compile_options(${item} PRIVATE ${CompileOptShared})

		set_property(TARGET ${item} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${ProjBinDir})
	else()
		add_library(${item} STATIC ${ARGN})
		target_compile_definitions(${item} PRIVATE IMP_AREG_LIB=1)
		target_compile_options(${item} PRIVATE ${CompileOptStatic})

		set_property(TARGET ${item} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${ProjLibDir})
	endif()

	# Linking flags
	target_link_libraries(${item} ${exampleLDFlags})

	# Adjusting CPP standard for target
	set_target_properties(${item}
		PROPERTIES
		CXX_STANDARD ${exampleCXXStandard}
		CXX_STANDARD_REQUIRED ON
		)

endfunction(setLibOpt item)

# Examples which has similar building structure
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

# Examples with specific design must be included one by one.
#include(${AREG_EXAMPLES}/00_helloservice/CMakeLists.txt)
include(${AREG_EXAMPLES}/10_locsvc/CMakeLists.txt)
#include(${AREG_EXAMPLES}/11_locmesh/CMakeLists.txt)
#include(${AREG_EXAMPLES}/12_pubsvc/CMakeLists.txt)
#include(${AREG_EXAMPLES}/13_pubmesh/CMakeLists.txt)
#include(${AREG_EXAMPLES}/14_pubtraffic/CMakeLists.txt)
#include(${AREG_EXAMPLES}/15_pubworker/CMakeLists.txt)
#include(${AREG_EXAMPLES}/16_pubfsm/CMakeLists.txt)


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
