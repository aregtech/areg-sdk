/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/NERemoteService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Remote service namespace
 ************************************************************************/

#include "areg/ipc/NERemoteService.hpp"
#include "areg/base/String.hpp"

AREG_API const char * NERemoteService::getServiceConnectionTypeString(NERemoteService::eServiceConnection connectionType)
{
    switch (connectionType)
    {
    case NERemoteService::eServiceConnection::ConnectionTcpip:
        return NERemoteService::STR_CONNECTION_TYPE_TCPIP.data();

    case NERemoteService::eServiceConnection::ConnectionUndefined:  // fall through
    default:
        return String::EmptyString.data();
    }
}

AREG_API NERemoteService::eServiceConnection NERemoteService::getServiceConnectionType(const char * connectionType, bool caseSensitive /* = true */ )
{
    if ( NEString::compareStrings<char, char>(NERemoteService::STR_CONNECTION_TYPE_TCPIP.data(), connectionType, NEString::COUNT_ALL, caseSensitive ) == 0)
        return NERemoteService::eServiceConnection::ConnectionTcpip;
    else
        return NERemoteService::eServiceConnection::ConnectionUndefined;
}
