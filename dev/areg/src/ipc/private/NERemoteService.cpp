/************************************************************************
 * \file        areg/src/ipc/private/NERemoteService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Remote service namespace
 ************************************************************************/

#include "areg/src/ipc/NERemoteService.hpp"

/************************************************************************
 * \file        areg/src/ipc/private/NERemoteService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Remote service namespace
 ************************************************************************/

#include "areg/src/ipc/NERemoteService.hpp"
#include "areg/src/base/CEString.hpp"

const char * NERemoteService::GetServiceConnectionTypeString(NERemoteService::eServiceConnection connectionType)
{
    switch (connectionType)
    {
    case NERemoteService::ConnectionTcpip:
        return NERemoteService::STR_CONNECTION_TYPE_TCPIP;
    default:
        return CEString::EmptyString;
    }
}

NERemoteService::eServiceConnection NERemoteService::GetServiceConnectionType(const char * connectionType, bool caseSensitive /* = true */ )
{
    if ( NEString::compareStrings<char, char>(NERemoteService::STR_CONNECTION_TYPE_TCPIP, connectionType, NEString::CountAll, caseSensitive ) == 0)
        return NERemoteService::ConnectionTcpip;
    else
        return NERemoteService::ConnectionUndefined;
}
