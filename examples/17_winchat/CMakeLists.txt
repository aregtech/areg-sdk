set(project_root "${CMAKE_CURRENT_LIST_DIR}")

# Set project to compile with MFC dynamic linked library
set(CMAKE_MFC_FLAG PRIVATE 2)

# ####################################
# generated project

# 'generated project'
DECLARE_PROJECT(17_generated    "generated")
# Setting up appropriate flags
addStaticLib(${project_generated} "${generated_src}")

# Set MFC DLL and other Windows flags.
list(APPEND MFC_DEFINES _AFXDLL _BIND_TO_CURRENT_CRT_VERSION _BIND_TO_CURRENT_MFC_VERSION)

# ####################################
# 'chatter' project

DECLARE_PROJECT(17_chatter      "chatter")
# Set resource files to compile with Microsoft RC compiler
set_source_files_properties(${chatter_resources}  PROPERTIES LANGUAGE RC)
list(APPEND chatter_src "${chatter_resources}")

# 'chatter' project, compile for Windows system, set options
add_executable(${project_chatter} WIN32 ${chatter_src})
setAppOptions(${project_chatter} ${project_generated})
# Set compile definitions to link MFC DLL
target_compile_definitions(${project_chatter} PRIVATE "${MFC_DEFINES}")
# Ensure Windows Subsystem and switch the entry point from main() method to 'wWinMainCRTStartup' implemented in MFC DLL.
set_property(TARGET ${project_chatter} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\"")
target_include_directories(${project_chatter}   BEFORE PRIVATE ${project_root})

# ####################################
# 'register' project

DECLARE_PROJECT(17_register     "register")
# Set resource files to compile with Microsoft RC compiler
set_source_files_properties(${register_resources} PROPERTIES LANGUAGE RC)
list(APPEND register_src ${register_resources})
# 'register' project, compile for Windows system, set options
add_executable(${project_register} WIN32 ${register_src})
setAppOptions(${project_register} ${project_generated})
# Set compile definitions to link MFC DLL
target_compile_definitions(${project_register} PRIVATE "${MFC_DEFINES}")
# Ensure Windows Subsystem and switch the entry point from main() method to 'wWinMainCRTStartup' implemented in MFC DLL.
set_property(TARGET ${project_register} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\"")
target_include_directories(${project_register}  BEFORE PRIVATE ${project_root})
