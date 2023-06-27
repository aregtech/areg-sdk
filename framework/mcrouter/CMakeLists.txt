set(mcrouter_BASE "${AREG_BASE}/mcrouter")
set(mcrouter_RESOURCE "${mcrouter_BASE}/resources")
set(mcrouter_SRC)

include("${mcrouter_BASE}/app/private/CMakeLists.txt")
include("${mcrouter_BASE}/tcp/private/CMakeLists.txt")
