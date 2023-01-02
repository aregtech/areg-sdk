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
include(${AREG_EXAMPLES}/20_pubdatarate/CMakeLists.txt)

# Iterating over projectList and extract each source file,
# to create separate target and executable.
# Iterating over projectList and extract each source file,
# to create separate target and executable.
foreach(item IN LISTS projectList)
	unset(SRC)
	set(itemDir ${AREG_EXAMPLES}/${item})
	set(subProjDir ${itemDir}/src)
	include(${AREG_EXAMPLES}/${item}/CMakeLists.txt)

	# Adding target for each example.
	addExecutable(${item} "${SRC}")
	target_include_directories(${item} BEFORE PRIVATE "${itemDir}")

endforeach()
