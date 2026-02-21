#pragma once

/************************************************************************
 * \file            register/CentralAppDefs.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/logging/GELog.h"

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
