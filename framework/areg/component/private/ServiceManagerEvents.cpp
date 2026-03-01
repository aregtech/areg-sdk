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
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventData class implementation
//////////////////////////////////////////////////////////////////////////

ServiceManagerEventData ServiceManagerEventData::stop_router_client()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient );
}

ServiceManagerEventData ServiceManagerEventData::shutdown_service_manager()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService );
}

ServiceManagerEventData ServiceManagerEventData::register_proxy(const ProxyAddress & addrProxy)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy );
    OutStream & stream = data.write_stream();
    stream << addrProxy;
    stream << addrProxy.channel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregister_proxy( const ProxyAddress & addrProxy, areg::DisconnectReason reason )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy );
    OutStream & stream = data.write_stream();
    stream << addrProxy;
    stream << addrProxy.channel();
    stream << reason;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::register_stub(const StubAddress & addrStub)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterStub );
    OutStream & stream = data.write_stream();
    stream << addrStub;
    stream << addrStub.channel();
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregister_stub( const StubAddress & addrStub, areg::DisconnectReason reason )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterStub );
    OutStream & stream = data.write_stream();
    stream << addrStub;
    stream << addrStub.channel();
    stream << reason;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::configure_connection(areg::RemoteServiceKind service, uint32_t connectTypes)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection );
    OutStream & stream = data.write_stream();
    stream << service;
    stream << connectTypes;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::start_connection(areg::RemoteServiceKind service, uint32_t connectTypes)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection );
    OutStream & stream = data.write_stream();
    stream << service;
    stream << connectTypes;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::start_net_connection( const String & ipAddress, uint16_t portNr )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection );
    OutStream & stream = data.write_stream( );
    stream << ipAddress;
    stream << portNr;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::stop_connection()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection );
}

ServiceManagerEventData ServiceManagerEventData::register_connection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection );
    OutStream & stream = data.write_stream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregister_connection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection );
    OutStream & stream = data.write_stream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::lost_connection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection );
    OutStream & stream = data.write_stream();
    stream << channel;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::terminate_component_thread(const String& threadName)
{
    ServiceManagerEventData data(ServiceManagerEventData::ServiceManagerCommand::CMD_TerminateComponentThread);
    OutStream& stream = data.write_stream();
    stream << threadName;
    return data;
}

ServiceManagerEventData ServiceManagerEventData::create_component_thread(const String& threadName)
{
    ServiceManagerEventData data(ServiceManagerEventData::ServiceManagerCommand::CMD_StartComponentThread);
    OutStream& stream = data.write_stream();
    stream << threadName;
    return data;
}

} // namespace areg
