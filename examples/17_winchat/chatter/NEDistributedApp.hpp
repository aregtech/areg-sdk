#pragma once

/************************************************************************
 * \file            chatter/NEDistributedApp.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"

namespace NEDistributedApp
{
    const char * const  THREAD_DISTRIBUTED      = "ThreadMessager";

    enum class eWndCommands : unsigned int
    {
          CmdFirst              = 0u

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
    };

    const char * const  PREFIX_TRHEAD       = "thread_";
    const char * const  PREFIX_MODEL        = "model_";
    const char * const  PREFIX_INITIATOR    = "init_";
    const char * const  PREFIX_PARTICIPANT  = "part_";

    String getDirectConnectionName( const String & nickName, uint32_t cookie, uint64_t session);

    String getConnectionServiceRole( const String & nickName, uint32_t cookie );

    String getDirectMessagingRole( const String & nickName, uint32_t cookie, uint64_t session, bool isInitiator );
}
