/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEvents.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Manager event for client.
 *
 ************************************************************************/
#include "areg/component/private/ServiceManagerEvents.hpp"

#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/private/ServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventData class implementation
//////////////////////////////////////////////////////////////////////////

ServiceManagerEventData ServiceManagerEventData::stopMessageRouterClient()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient );
}

ServiceManagerEventData ServiceManagerEventData::shutdownServiceManager()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService );
}

ServiceManagerEventData ServiceManagerEventData::registerProxy(const ProxyAddress & addrProxy)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy );
    OutStream & stream = data.getWriteStream();
    stream << addrProxy;
    stream << addrProxy.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterProxy( const ProxyAddress & addrProxy, NEService::DisconnectReason reason )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy );
    OutStream & stream = data.getWriteStream();
    stream << addrProxy;
    stream << addrProxy.getChannel();
    stream << reason;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::registerStub(const StubAddress & addrStub)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterStub );
    OutStream & stream = data.getWriteStream();
    stream << addrStub;
    stream << addrStub.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterStub( const StubAddress & addrStub, NEService::DisconnectReason reason )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterStub );
    OutStream & stream = data.getWriteStream();
    stream << addrStub;
    stream << addrStub.getChannel();
    stream << reason;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::configureConnection(NERemoteService::RemoteServiceKind service, uint32_t connectTypes)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection );
    OutStream & stream = data.getWriteStream();
    stream << service;
    stream << connectTypes;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::startConnection(NERemoteService::RemoteServiceKind service, uint32_t connectTypes)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection );
    OutStream & stream = data.getWriteStream();
    stream << service;
    stream << connectTypes;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::startNetConnection( const String & ipAddress, uint16_t portNr )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection );
    OutStream & stream = data.getWriteStream( );
    stream << ipAddress;
    stream << portNr;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::stopConnection()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection );
}

ServiceManagerEventData ServiceManagerEventData::registerConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection );
    OutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection );
    OutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::lostConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection );
    OutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::terminateComponentThread(const String& threadName)
{
    ServiceManagerEventData data(ServiceManagerEventData::ServiceManagerCommand::CMD_TerminateComponentThread);
    OutStream& stream = data.getWriteStream();
    stream << threadName;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::createComponentThread(const String& threadName)
{
    ServiceManagerEventData data(ServiceManagerEventData::ServiceManagerCommand::CMD_StartComponentThread);
    OutStream& stream = data.getWriteStream();
    stream << threadName;
    return data;
}

