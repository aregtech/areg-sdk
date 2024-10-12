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

macro_declare_executable(17_chatter 
    17_generated
    chatter/res/stdafx.cpp
    chatter/res/chatter.rc

    chatter/DistrbutedApp.cpp
    chatter/NEDistributedApp.cpp

    chatter/services/CentralMessaging.cpp
    chatter/services/ChatParticipantService.cpp
    chatter/services/ChatPrticipantHandler.cpp
    chatter/services/ConnectionHandler.cpp
    chatter/services/ConnectionList.cpp
    chatter/services/ConnectionService.cpp
    chatter/services/DirectChatService.cpp
    chatter/services/DirectConnectionClient.cpp
    chatter/services/DirectConnectionService.cpp
    chatter/services/DirectMessagingClient.cpp
    chatter/services/NetworkSetup.cpp

    chatter/ui/DistributedDialog.cpp
    chatter/ui/PageChat.cpp
    chatter/ui/PageConnections.cpp
    chatter/ui/PageMessaging.cpp
    chatter/ui/PageNetworkSetup.cpp
)

# Ensure Windows Subsystem and switch the entry point from main() method to 'wWinMainCRTStartup' implemented in MFC DLL.
set_target_properties(17_chatter PROPERTIES 
        WIN32_EXECUTABLE    TRUE
        LINK_FLAGS          " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\""
)
# Set compile definitions to link MFC DLL
target_compile_definitions(17_chatter PRIVATE "${MFC_DEFINES}")
target_compile_options(17_chatter PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")

# ####################################
# 'register' project


macro_declare_executable(17_register    17_generated
    register/res/stdafx.cpp
    register/res/register.rc

    register/CentralApp.cpp
    register/NECentralApp.cpp

    register/services/ConnectionManager.cpp
    
    register/ui/CentralDialog.cpp
    register/ui/PageBrokerSetup.cpp
    register/ui/PageConnections.cpp
)

# Ensure Windows Subsystem and switch the entry point from main() method to 'wWinMainCRTStartup' implemented in MFC DLL.
set_target_properties(17_register PROPERTIES 
        WIN32_EXECUTABLE    TRUE
        LINK_FLAGS          " /SUBSYSTEM:WINDOWS /ENTRY:\"wWinMainCRTStartup\""
)

# Set compile definitions to link MFC DLL
target_compile_definitions(17_register PRIVATE "${MFC_DEFINES}")
target_compile_options(17_register PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")
