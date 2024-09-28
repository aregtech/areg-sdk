set(extend_BASE "${AREG_BASE}/extend")
set(extend_SRC)

include("${extend_BASE}/console/private/CMakeLists.txt")
include("${extend_BASE}/db/private/CMakeLists.txt")
include("${extend_BASE}/service/private/CMakeLists.txt")

# build areg extended static library
addStaticLib(areg-extend "${extend_SRC}")
target_compile_options(areg-extend PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
