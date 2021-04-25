#pragma once

/************************************************************************
 * \file            register/NECentralApp.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"

#include <windows.h>
#include <tchar.h>

namespace NECentralApp
{
    const char * const  THREAD_CENTRAL      = "ThreadCentral";

    typedef enum E_WndCommands
    {
          CmdFirst              = WM_USER + 10
        , CmdServiceConnection
        , CmdRegistered
        , CmdUnregistered
        , CmdSendMessage
        , CmdTypeMessage

        , CmdLast
    } eWndCommands;
}
