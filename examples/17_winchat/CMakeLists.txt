# SRC lists
unset(17_generated_SRC)
unset(17_SRC1)
unset(17_SRC1)
unset(17_RES1)
unset(17_RES2)

# Project names
set(gen_projName 17_generated)
set(projName1 17_chatter)
set(projName2 17_register)
set(17_winchat_BASE "${AREG_EXAMPLES}/17_winchat")

# Filling SRCs up
include(${17_winchat_BASE}/generated/CMakeLists.txt)
include(${17_winchat_BASE}/chatter/CMakeLists.txt)
include(${17_winchat_BASE}/register/CMakeLists.txt)

# Set project to compile with MFC dynamic linked library
set(CMAKE_MFC_FLAG PRIVATE 2)

# Set resource files to compile with Microsoft RC compiler
set_source_files_properties(${17_RES1} PROPERTIES LANGUAGE RC)
set_source_files_properties(${17_RES2} PROPERTIES LANGUAGE RC)

list(APPEND 17_SRC1 ${17_RES1})
list(APPEND 17_SRC2 ${17_RES2})

# Set MFC DLL and other Windows flags.
list(APPEND MFC_DEFINES _AFXDLL _BIND_TO_CURRENT_CRT_VERSION _BIND_TO_CURRENT_MFC_VERSION)

# Setting up appropriate flags
addStaticLib(${gen_projName} "${17_generated_SRC}")

# target: 17_chatter, compile for Windows system, set options
add_executable(${projName1} WIN32 ${17_SRC1})
setAppOptions(${projName1} ${gen_projName})

# target: 17_register, compile for Windows system, set options
add_executable(${projName2} WIN32 ${17_SRC2})
setAppOptions(${projName2} ${gen_projName})

# Set compile definitions to link MFC DLL
target_compile_definitions(${projName1} PRIVATE "${MFC_DEFINES}")
target_compile_definitions(${projName2} PRIVATE "${MFC_DEFINES}")

# Ensure Windows Subsystem and switch the entry point from main() method to 'wWinMainCRTStartup' implemented in MFC DLL.
set_property(TARGET ${projName1} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\"")
set_property(TARGET ${projName2} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\"")

target_include_directories(${gen_projName} BEFORE PRIVATE ${17_winchat_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${17_winchat_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${17_winchat_BASE})
