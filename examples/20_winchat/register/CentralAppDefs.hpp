#pragma once

/************************************************************************
 * \file            register/CentralAppDefs.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/logging/areg_log.h"

namespace NECentralApp
{
    const char * const  THREAD_CENTRAL      = "ThreadCentral";

    enum class WindowCommand : uint32_t
    {
          CmdFirst
        , CmdServiceConnection
        , CmdRegistered
        , CmdUnregistered
        , CmdSendMessage
        , CmdTypeMessage

        , CmdLast
    } ;
}
