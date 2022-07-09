set(mcrouter_PROJECT_NAME "mcrouter")

set(mcrouter_BASE "${AREG_BASE}/mcrouter")

set(mcrouter_TARGET_BIN)

set(mcrouter_SRC)
set(mcrouter_OBJS)

include("${mcrouter_BASE}/app/private/CMakeLists.txt")
include("${mcrouter_BASE}/tcp/private/CMakeLists.txt")

set(mcrouter_LDFLAGS)
set(mcrouter_CXXFLAGS)

if(AREG_BINARY STREQUAL "shared")
    set(mcrouter_CXXFLAGS += "-D_IMP_AREG_DLL=1")
    set(mcrouter_LDFLAGS  += "-Wl,-rpath=${AREG_OUTPUT_BIN} -L ${AREG_OUTPUT_BIN} -lareg ${LDFLAGS}")
else()
	set(mcrouter_CXXFLAGS += "-D_IMP_AREG_LIB=1")
	set(mcrouter_LDFLAGS  += "-L ${AREG_OUTPUT_LIB} -Wl,-Bstatic -lareg -Wl,-Bdynamic ${LDFLAGS}}")
endif()

set(mcrouter_TARGET_BIN "${mcrouter_PROJECT_NAME}${AREG_BIN_EXT}")
