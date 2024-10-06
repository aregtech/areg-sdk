set(project_root "${CMAKE_CURRENT_LIST_DIR}")

# Set project to compile with MFC dynamic linked library
set(CMAKE_MFC_FLAG PRIVATE 2)

# ####################################
# add service interfaces
addServiceInterface(17_generated res CentralMessager)
addServiceInterface(17_generated res ConnectionManager)
addServiceInterface(17_generated res DirectConnection)
addServiceInterface(17_generated res DirectMessager)

# Set MFC DLL and other Windows flags.
list(APPEND MFC_DEFINES _AFXDLL _BIND_TO_CURRENT_CRT_VERSION _BIND_TO_CURRENT_MFC_VERSION)

# ####################################
# 'chatter' project

macro_declare_project(17_chatter      "chatter")
# Set resource files to compile with Microsoft RC compiler
set_source_files_properties(${chatter_resources}  PROPERTIES LANGUAGE RC)
list(APPEND chatter_src "${chatter_resources}")

# 'chatter' project, compile for Windows system, set options
addExecutableEx(${project_chatter} "" "${chatter_src}" 17_generated)
# Set compile definitions to link MFC DLL
set_target_properties(${project_chatter} PROPERTIES WIN32_EXECUTABLE TRUE)
target_compile_definitions(${project_chatter} PRIVATE "${MFC_DEFINES}")
# Ensure Windows Subsystem and switch the entry point from main() method to 'wWinMainCRTStartup' implemented in MFC DLL.
set_property(TARGET ${project_chatter} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\"")
target_include_directories(${project_chatter} BEFORE PRIVATE ${project_root})
target_compile_options(${project_chatter} PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")

# ####################################
# 'register' project

macro_declare_project(17_register     "register")
# Set resource files to compile with Microsoft RC compiler
set_source_files_properties(${register_resources} PROPERTIES LANGUAGE RC)
list(APPEND register_src ${register_resources})
# 'register' project, compile for Windows system, set options
addExecutableEx(${project_register} "" "${register_src}" 17_generated)
# Set compile definitions to link MFC DLL
set_target_properties(${project_register} PROPERTIES WIN32_EXECUTABLE TRUE)
target_compile_definitions(${project_register} PRIVATE "${MFC_DEFINES}")
# Ensure Windows Subsystem and switch the entry point from main() method to 'wWinMainCRTStartup' implemented in MFC DLL.
set_property(TARGET ${project_register} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\"")
target_include_directories(${project_register}  BEFORE PRIVATE ${project_root})
target_compile_options(${project_register} PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")
