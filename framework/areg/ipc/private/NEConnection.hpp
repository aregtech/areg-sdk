#ifndef AREG_IPC_PRIVATE_NECONNECTION_HPP
#define AREG_IPC_PRIVATE_NECONNECTION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/NEConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Connection constants.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"

#include <string_view>

//////////////////////////////////////////////////////////////////////////
// NEConnection namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connection namespace with shared objects and helper methods
 *          to create fixed communication messages.
 **/
namespace NEConnection
{
    /**
     * \brief   NEConnection::CLIENT_SEND_MESSAGE_THREAD
     *          Fixed name of client message sender thread
     **/
    constexpr std::string_view  CLIENT_SEND_MESSAGE_THREAD      { "CLIENT_SEND_MESSAGE_THREAD" };
    /**
     * \brief   NEConnection::CLIENT_RECEIVE_MESSAGE_THREAD
     *          Fixed name of client message receiver thread
     **/
    constexpr std::string_view  CLIENT_RECEIVE_MESSAGE_THREAD   { "CLIENT_RECEIVE_MESSAGE_THREAD" };
    /**
     * \brief   NEConnection::CLIENT_DISPATCH_MESSAGE_THREAD
     *          Fixed name of client message dispatcher thread
     **/
    constexpr std::string_view  CLIENT_DISPATCH_MESSAGE_THREAD  { "CLIENT_DISPATCH_MESSAGE_THREAD" };
    /**
     * \brief   NEConnection::SERVER_SEND_MESSAGE_THREAD
     *          Fixed name of server message sender thread
     **/
    constexpr std::string_view  SERVER_SEND_MESSAGE_THREAD      { "SERVER_SEND_MESSAGE_THREAD" };
    /**
     * \brief   NEConnection::SERVER_RECEIVE_MESSAGE_THREAD
     *          Fixed name of server message receiver thread
     **/
    constexpr std::string_view  SERVER_RECEIVE_MESSAGE_THREAD   { "SERVER_RECEIVE_MESSAGE_THREAD" };
    /**
     * \brief   NEConnection::SERVER_DISPATCH_MESSAGE_THREAD
     *          Fixed name of server message dispatcher thread
     **/
    constexpr std::string_view  SERVER_DISPATCH_MESSAGE_THREAD  { "SERVER_DISPATCH_MESSAGE_THREAD" };
    /**
     * \brief   NEConnection::CLIENT_CONNECT_TIMER_NAME
     *          Fixed name of client connection retry timer name
     **/
    constexpr std::string_view  CLIENT_CONNECT_TIMER_NAME       { "CLIENT_CONNECT_TIMER_NAME" };
    /**
     * \brief   NEConnection::SERVER_CONNECT_TIMER_NAME
     *          Fixed name of server connection retry timer name
     **/
    constexpr std::string_view  SERVER_CONNECT_TIMER_NAME       { "SERVER_CONNECT_TIMER_NAME" };
    /**
     * \brief   NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT
     *          Default connect retry timer timeout value in milliseconds
     **/
    constexpr unsigned int      DEFAULT_RETRY_CONNECT_TIMEOUT   { NECommon::TIMEOUT_500_MS };  // 500 ms
}

#endif  // AREG_IPC_NECONNECTION_HPP
