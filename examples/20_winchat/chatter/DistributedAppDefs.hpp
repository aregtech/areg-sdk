#pragma once

/************************************************************************
 * \file            chatter/DistributedAppDefs.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/logging/areg_log.h"

namespace NEDistributedApp
{
    const char * const  THREAD_DISTRIBUTED      = "ThreadMessager";

    enum class WindowCommand : uint32_t
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

    areg::String getDirectConnectionName( const areg::String & nickName, uint32_t cookie, uint64_t session);

    areg::String getConnectionServiceRole( const areg::String & nickName, uint32_t cookie );

    areg::String getDirectMessagingRole( const areg::String & nickName, uint32_t cookie, uint64_t session, bool isInitiator );
}
