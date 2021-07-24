/************************************************************************
 * \file        areg/component/private/ServiceManagerEvents.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    return ServiceManagerEventData( ServiceManagerEventData::CMD_StopRoutingClient );
}

ServiceManagerEventData ServiceManagerEventData::shutdownServiceManager(void)
{
    return ServiceManagerEventData( ServiceManagerEventData::CMD_ShutdownService );
}

ServiceManagerEventData ServiceManagerEventData::registerProxy(const ProxyAddress & addrProxy)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_RegisterProxy );
    IEOutStream & stream = data.getWriteStream();
    stream << addrProxy;
    stream << addrProxy.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterProxy(const ProxyAddress & addrProxy)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_UnregisterProxy );
    IEOutStream & stream = data.getWriteStream();
    stream << addrProxy;
    stream << addrProxy.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::registerStub(const StubAddress & addrStub)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_RegisterStub );
    IEOutStream & stream = data.getWriteStream();
    stream << addrStub;
    stream << addrStub.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterStub(const StubAddress & addrStub)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_UnregisterStub );
    IEOutStream & stream = data.getWriteStream();
    stream << addrStub;
    stream << addrStub.getChannel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::configureConnection(const String & configFile)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_ConfigureConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << configFile;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::startConnection(const String & configFile)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_StartConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << configFile;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::startNetConnection( const String & ipAddress, unsigned short portNr )
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_StartNetConnection );
    IEOutStream & stream = data.getWriteStream( );
    stream << ipAddress;
    stream << portNr;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::stopConnection(void)
{
    return ServiceManagerEventData( ServiceManagerEventData::CMD_StopConnection );
}

ServiceManagerEventData ServiceManagerEventData::enableRemoteService( bool enable )
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_SetEnableService );
    IEOutStream & stream = data.getWriteStream( );
    stream << enable;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::registerConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_RegisterConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregisterConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_UnregisterConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::lostConnection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::CMD_LostConnection );
    IEOutStream & stream = data.getWriteStream();
    stream << channel;
    return data;
}

