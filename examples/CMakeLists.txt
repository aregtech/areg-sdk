if($(AREG_BINARY) MATCHES "shared")
    #These variables must be included in CMAKE_CXX_FLAGS
	set(examples_CXXFLAGS "-DIMP_AREG_DLL")
	set(examples_LDFLAGS  "Wl,-rpath=${AREG_OUTPUT_BIN} -L ${AREG_OUTPUT_BIN} -lareg ${LDFLAGS}")
else()
    #These variables must be included in CMAKE_CXX_FLAGS
	set(examples_CXXFLAGS "-DIMP_AREG_LIB")
	set(examples_LDFLAGS  "-L ${AREG_OUTPUT_LIB} -Wl,-Bstatic -lareg -Wl,-Bdynamic ${LDFLAGS}")
endif()