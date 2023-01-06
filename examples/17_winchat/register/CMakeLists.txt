set(subProjDir ${17_winchat_BASE}/register)

include(${subProjDir}/res/CMakeLists.txt)
include(${subProjDir}/services/CMakeLists.txt)
include(${subProjDir}/ui/CMakeLists.txt)

list(APPEND 17_SRC2
    ${subProjDir}/CentralApp.cpp
    ${subProjDir}/NECentralApp.cpp
)
