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
