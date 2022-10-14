set(mcrouter_PROJECT_NAME "mcrouter")

set(mcrouter_BASE "${AREG_BASE}/mcrouter")

set(mcrouter_TARGET_BIN)

set(mcrouter_SRC)

include("${mcrouter_BASE}/app/private/CMakeLists.txt")
include("${mcrouter_BASE}/tcp/private/CMakeLists.txt")

set(mcrouter_TARGET_BIN "${mcrouter_PROJECT_NAME}${AREG_BIN_EXT}")
