set(subProjDir ${17_winchat_BASE}/chatter)

include(${subProjDir}/res/CMakeLists.txt)
include(${subProjDir}/services/CMakeLists.txt)
include(${subProjDir}/ui/CMakeLists.txt)

list(APPEND 17_SRC1
    ${subProjDir}/DistrbutedApp.cpp
    ${subProjDir}/NEDistributedApp.cpp
)
