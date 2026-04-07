#ifndef AREG_IPC_PRIVATE_CONNECTIONDEFS_HPP
#define AREG_IPC_PRIVATE_CONNECTIONDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ConnectionDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Connection constants.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"

#include <string_view>

//////////////////////////////////////////////////////////////////////////
// Connection namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connection namespace with shared objects and helper methods
 *          to create fixed communication messages.
 **/
namespace areg {
    /**
     * \brief   areg::CLIENT_SEND_MESSAGE_THREAD
     *          Fixed name of client message sender thread
     **/
    constexpr std::string_view  CLIENT_SEND_MESSAGE_THREAD      { "CLIENT_SEND_MESSAGE_THREAD" };
    /**
     * \brief   areg::CLIENT_RECEIVE_MESSAGE_THREAD
     *          Fixed name of client message receiver thread
     **/
    constexpr std::string_view  CLIENT_RECEIVE_MESSAGE_THREAD   { "CLIENT_RECEIVE_MESSAGE_THREAD" };
    /**
     * \brief   areg::CLIENT_DISPATCH_MESSAGE_THREAD
     *          Fixed name of client message dispatcher thread
     **/
    constexpr std::string_view  CLIENT_DISPATCH_MESSAGE_THREAD  { "CLIENT_DISPATCH_MESSAGE_THREAD" };
    /**
     * \brief   areg::SERVER_SEND_MESSAGE_THREAD
     *          Fixed name of server message sender thread
     **/
    constexpr std::string_view  SERVER_SEND_MESSAGE_THREAD      { "SERVER_SEND_MESSAGE_THREAD" };
    /**
     * \brief   areg::SERVER_RECEIVE_MESSAGE_THREAD
     *          Fixed name of server message receiver thread
     **/
    constexpr std::string_view  SERVER_RECEIVE_MESSAGE_THREAD   { "SERVER_RECEIVE_MESSAGE_THREAD" };
    /**
     * \brief   areg::SERVER_DISPATCH_MESSAGE_THREAD
     *          Fixed name of server message dispatcher thread
     **/
    constexpr std::string_view  SERVER_DISPATCH_MESSAGE_THREAD  { "SERVER_DISPATCH_MESSAGE_THREAD" };
    /**
     * \brief   areg::CLIENT_CONNECT_TIMER_NAME
     *          Fixed name of client connection retry timer name
     **/
    constexpr std::string_view  CLIENT_CONNECT_TIMER_NAME       { "CLIENT_CONNECT_TIMER_NAME" };
    /**
     * \brief   areg::SERVER_CONNECT_TIMER_NAME
     *          Fixed name of server connection retry timer name
     **/
    constexpr std::string_view  SERVER_CONNECT_TIMER_NAME       { "SERVER_CONNECT_TIMER_NAME" };
    /**
     * \brief   areg::DEFAULT_RETRY_CONNECT_TIMEOUT
     *          Default connect retry timer timeout value in milliseconds
     **/
    constexpr uint32_t      DEFAULT_RETRY_CONNECT_TIMEOUT       { TIMEOUT_500_MS };  // 500 ms

    /**
     * \brief   areg::SEND_THREAD_QUEUE_LIMIT
     *          Maximum number of events queued in the server/client send thread.
     **/
    constexpr uint32_t      SEND_THREAD_QUEUE_LIMIT             { 256u };

} // namespace areg
#endif  // AREG_IPC_PRIVATE_CONNECTIONDEFS_HPP
