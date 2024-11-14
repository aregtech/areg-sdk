#pragma once

/************************************************************************
 * \file            register/NECentralApp.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/logging/GELog.h"

namespace NECentralApp
{
    const char * const  THREAD_CENTRAL      = "ThreadCentral";

    enum class eWndCommands : unsigned int
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
