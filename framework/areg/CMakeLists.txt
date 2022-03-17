
set(areg_BASE "${AREG_BASE}/areg")
set(areg_OUTPUT_OBJ "${AREG_OUTPUT_OBJ}/${PROJECT_NAME}")
set(areg_OUTPUT_LIB "${AREG_BASE}/areg") 
set(areg_OUTPUT_BIN "${AREG_BASE}/areg")

set(areg_OBJS)

include("${areg_BASE}/appbase/private/CMakeLists.txt")
include("${areg_BASE}/base/private/CMakeLists.txt")
include("${areg_BASE}/component/private/CMakeLists.txt")
include("${areg_BASE}/ipc/private/CMakeLists.txt")
include("${areg_BASE}/persist/private/CMakeLists.txt")
include("${areg_BASE}/trace/private/CMakeLists.txt")

if(AREG_BINARY STREQUAL "shared")
    set(areg_CXXFLAGS "${areg_CXXFLAGS} -fPIC -DEXP_AREG_DLL")
else()
    set(areg_CXXFLAGS "${areg_CXXFLAGS} -DEXP_AREG_LIB")
endif()

set(areg_TARGET_PATH "${areg_OUTPUT_BIN}/${AREG_LIB_PREFIX}/${PROJECT_NAME}/${AREG_LIB_EXT}")

