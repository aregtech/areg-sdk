#pragma once

/************************************************************************
 * \file            edge/NEDistributedApp.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/src/base/GEGlobal.h"
#include "areg/src/trace/GETrace.h"

#include <windows.h>
#include <tchar.h>

namespace NEDistributedApp
{
    const char * const  THREAD_DISTRIBUTED      = "ThreadMessager";

    typedef enum E_WndCommands
    {
          CmdFirst              = WM_USER + 10

        , CmdServiceStartup
        , CmdServiceNetwork
        , CmdServiceConnection
        , CmdClientConnection
        , CmdClientRegistration

        , CmdAddConnection
        , CmdRemoveConnection
        , CmdUpdateConnection
        , CmdDisconnectTriggered

        , CmdSendMessage
        , CmdTypeMessage

        , CmdSetDirectConnection
        , CmdDirectConnectionAdd
        , CmdDirectConnectionRemove
        , CmdDirectConnectionClose

        , CmdChatJoined
        , CmdChatMessage
        , CmdChatTyping
        , CmdChatClosed

        , CmdLast
    } eWndCommands;

    const char * const  PREFIX_TRHEAD       = "thread_";
    const char * const  PREFIX_MODEL        = "model_";
    const char * const  PREFIX_INITIATOR    = "init_";
    const char * const  PREFIX_PARTICIPANT  = "part_";

    CEString getDirectConnectionName( const CEString & nickName, uint32_t cookie, uint64_t session);

    CEString getConnectionServiceRole( const CEString & nickName, uint32_t cookie );

    CEString getDirectMessagingRole( const CEString & nickName, uint32_t cookie, uint64_t session, bool isInitiator );
}
