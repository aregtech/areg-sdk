
function(addTestEx testSource testName)

	add_executable(${testName} ${testSource})

	if(AREG_BINARY MATCHES shared)

		if(AREG_OS MATCHES "Windows")
			target_compile_definitions(${testName} PRIVATE IMP_AREG_DLL)
		else()
			target_compile_definitions(${testName} PRIVATE IMP_AREG_DLL GTEST_HAS_PTHREAD=1)
		endif()

		target_compile_options(${testName} PRIVATE ${ExampleCompileOptShared})
	else()
		if(AREG_OS MATCHES "Windows")
			target_compile_definitions(${testName} PRIVATE IMP_AREG_LIB)
		else()
			target_compile_definitions(${testName} PRIVATE IMP_AREG_LIB GTEST_HAS_PTHREAD=1)
		endif()

		target_compile_options(${testName} PRIVATE ${ExampleCompileOptStatic})
	endif()

	# Linking flags
	target_link_libraries(${testName} ${AREG_EXAMPLES_LDFLAGS} GTest::gtest_main)

	# Adjusting CPP standard for target
	set_target_properties(${testName}
		PROPERTIES
		CXX_STANDARD ${AREG_CXX_STANDARD}
		CXX_STANDARD_REQUIRED ON
		)

	gtest_discover_tests(${testName})
	add_test(aregUnitTests ${testName})

endfunction(addTestEx testSrc, testName)

function(addTest testName)
	addTestEx(${AREG_UNIT_TEST_BASE}/${testName} ${testName})
endfunction(addTest testName)

include("${AREG_UNIT_TEST_BASE}/CMakeLists.txt")
