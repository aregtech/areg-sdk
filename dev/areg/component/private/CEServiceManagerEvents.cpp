/************************************************************************
 * \file        areg/component/private/CEServiceManagerEvents.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Manager event for client.
 *
 ************************************************************************/
#include "areg/component/private/CEServiceManagerEvents.hpp"

#include "areg/component/CEStubAddress.hpp"
#include "areg/component/CEProxyAddress.hpp"
#include "areg/component/NEService.hpp"
#include "areg/component/private/CEServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// CEServiceManagerEventData class implementation
//////////////////////////////////////////////////////////////////////////

CEServiceManagerEventData CEServiceManagerEventData::StopMessageRouterClient(void)
{
    return CEServiceManagerEventData( CEServiceManagerEventData::CMD_StopRoutingClient );
}

CEServiceManagerEventData CEServiceManagerEventData::RegisterProxy(const CEProxyAddress & addrProxy)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_RegisterProxy );
    IEOutStream & stream = data.GetWriteStream();
    stream << addrProxy;
    stream << addrProxy.GetChannel();
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::UnregisterProxy(const CEProxyAddress & addrProxy)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_UnregisterProxy );
    IEOutStream & stream = data.GetWriteStream();
    stream << addrProxy;
    stream << addrProxy.GetChannel();
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::RegisterStub(const CEStubAddress & addrStub)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_RegisterStub );
    IEOutStream & stream = data.GetWriteStream();
    stream << addrStub;
    stream << addrStub.GetChannel();
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::UnregisterStub(const CEStubAddress & addrStub)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_UnregisterStub );
    IEOutStream & stream = data.GetWriteStream();
    stream << addrStub;
    stream << addrStub.GetChannel();
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::ConfigureConnection(const CEString & configFile)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_ConfigureConnection );
    IEOutStream & stream = data.GetWriteStream();
    stream << configFile;
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::StartConnection(const CEString & configFile)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_StartConnection );
    IEOutStream & stream = data.GetWriteStream();
    stream << configFile;
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::StartNetConnection( const CEString & ipAddress, unsigned short portNr )
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_StartNetConnection );
    IEOutStream & stream = data.GetWriteStream( );
    stream << ipAddress;
    stream << portNr;
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::StopConnection(void)
{
    return CEServiceManagerEventData( CEServiceManagerEventData::CMD_StopConnection );
}

CEServiceManagerEventData CEServiceManagerEventData::EnableRemoteService( bool enable )
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_SetEnableService );
    IEOutStream & stream = data.GetWriteStream( );
    stream << enable;
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::RegisterConnection(const CEChannel & channel)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_RegisterConnection );
    IEOutStream & stream = data.GetWriteStream();
    stream << channel;
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::UnregisterConnection(const CEChannel & channel)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_UnregisterConnection );
    IEOutStream & stream = data.GetWriteStream();
    stream << channel;
    return data;
}

CEServiceManagerEventData CEServiceManagerEventData::LostConnection(const CEChannel & channel)
{
    CEServiceManagerEventData data( CEServiceManagerEventData::CMD_LostConnection );
    IEOutStream & stream = data.GetWriteStream();
    stream << channel;
    return data;
}

