/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEvents.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Manager event for client.
 *
 ************************************************************************/
#include "areg/component/private/ServiceManagerEvents.hpp"

#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/NEService.hpp"
#include "areg/component/private/ServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventData class implementation
//////////////////////////////////////////////////////////////////////////

ServiceManagerEventData ServiceManagerEventData::stopMessageRouterClient(void)
{
    return ServiceManagerEventData( ServiceManagerEventData::eServiceManagerCommands::CMD_StopRoutingClient );
}

ServiceManagerEventData ServiceManagerEventData::shutdownServiceManager(void)
{
    return ServiceManagerEventData( ServiceManagerEventData::eServiceManagerCommands::CMD_ShutdownService );
}

ServiceManagerEventData ServiceManagerEventData::registerProxy(const ProxyAddress & addrProxy)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterProxy );
    IEOutStream & stream = data.getWriteStream();
    stream << addrProxy;
    stream << addrProxy.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterProxy(const ProxyAddress & addrProxy)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterProxy );
    IEOutStream & stream = data.getWriteStream();
    stream << addrProxy;
    stream << addrProxy.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::registerStub(const StubAddress & addrStub)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterStub );
    IEOutStream & stream = data.getWriteStream();
    stream << addrStub;
    stream << addrStub.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterStub(const StubAddress & addrStub)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterStub );
    IEOutStream & stream = data.getWriteStream();
    stream << addrStub;
    stream << addrStub.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::configureConnection(const String & configFile)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_ConfigureConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << configFile;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::startConnection(const String & configFile)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_StartConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << configFile;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::startNetConnection( const String & ipAddress, unsigned short portNr )
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_StartNetConnection );
    IEOutStream & stream = data.getWriteStream( );
    stream << ipAddress;
    stream << portNr;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::stopConnection(void)
{
    return ServiceManagerEventData( ServiceManagerEventData::eServiceManagerCommands::CMD_StopConnection );
}

ServiceManagerEventData ServiceManagerEventData::enableRemoteService( bool enable )
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_SetEnableService );
    IEOutStream & stream = data.getWriteStream( );
    stream << enable;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::registerConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::lostConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::eServiceManagerCommands::CMD_LostConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::terminateComponentThread(const String& threadName)
{
    ServiceManagerEventData data(ServiceManagerEventData::eServiceManagerCommands::CMD_TerminateComponentThread);
    IEOutStream& stream = data.getWriteStream();
    stream << threadName;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::createComponentThread(const String& threadName)
{
    ServiceManagerEventData data(ServiceManagerEventData::eServiceManagerCommands::CMD_StartComponentThread);
    IEOutStream& stream = data.getWriteStream();
    stream << threadName;
    return data;
}

