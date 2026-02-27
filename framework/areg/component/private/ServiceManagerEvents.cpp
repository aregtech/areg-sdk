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

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // ServiceManagerEventData class implementation
    //////////////////////////////////////////////////////////////////////////

    areg::ServiceManagerEventData areg::ServiceManagerEventData::stopMessageRouterClient()
    {
        return areg::ServiceManagerEventData( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient );
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::shutdownServiceManager()
    {
        return areg::ServiceManagerEventData( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService );
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::registerProxy(const areg::ProxyAddress & addrProxy)
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy );
        areg::OutStream & stream = data.getWriteStream();
        stream << addrProxy;
        stream << addrProxy.getChannel();
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::unregisterProxy( const areg::ProxyAddress & addrProxy, areg::DisconnectReason reason )
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy );
        areg::OutStream & stream = data.getWriteStream();
        stream << addrProxy;
        stream << addrProxy.getChannel();
        stream << reason;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::registerStub(const areg::StubAddress & addrStub)
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterStub );
        areg::OutStream & stream = data.getWriteStream();
        stream << addrStub;
        stream << addrStub.getChannel();
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::unregisterStub( const areg::StubAddress & addrStub, areg::DisconnectReason reason )
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterStub );
        areg::OutStream & stream = data.getWriteStream();
        stream << addrStub;
        stream << addrStub.getChannel();
        stream << reason;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::configureConnection(areg::RemoteServiceKind service, uint32_t connectTypes)
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection );
        areg::OutStream & stream = data.getWriteStream();
        stream << service;
        stream << connectTypes;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::startConnection(areg::RemoteServiceKind service, uint32_t connectTypes)
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection );
        areg::OutStream & stream = data.getWriteStream();
        stream << service;
        stream << connectTypes;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::startNetConnection( const areg::String & ipAddress, uint16_t portNr )
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection );
        areg::OutStream & stream = data.getWriteStream( );
        stream << ipAddress;
        stream << portNr;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::stopConnection()
    {
        return areg::ServiceManagerEventData( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection );
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::registerConnection(const areg::Channel & channel)
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection );
        areg::OutStream & stream = data.getWriteStream();
        stream << channel;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::unregisterConnection(const areg::Channel & channel)
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection );
        areg::OutStream & stream = data.getWriteStream();
        stream << channel;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::lostConnection(const areg::Channel & channel)
    {
        areg::ServiceManagerEventData data( areg::ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection );
        areg::OutStream & stream = data.getWriteStream();
        stream << channel;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::terminateComponentThread(const areg::String& threadName)
    {
        areg::ServiceManagerEventData data(areg::ServiceManagerEventData::ServiceManagerCommand::CMD_TerminateComponentThread);
        areg::OutStream& stream = data.getWriteStream();
        stream << threadName;
        return data;
    }

    areg::ServiceManagerEventData areg::ServiceManagerEventData::createComponentThread(const areg::String& threadName)
    {
        areg::ServiceManagerEventData data(areg::ServiceManagerEventData::ServiceManagerCommand::CMD_StartComponentThread);
        areg::OutStream& stream = data.getWriteStream();
        stream << threadName;
        return data;
    }

} // namespace areg