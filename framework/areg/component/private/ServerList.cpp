/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServerList.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Server List class implementation
 *
 ************************************************************************/
#include "areg/component/private/ServerList.hpp"

#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// ServerList class implementation
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(areg_component_private_ServerList_findServer);
DEF_TRACE_SCOPE(areg_component_private_ServerList_registerClient);
DEF_TRACE_SCOPE(areg_component_private_ServerList_unregisterClient);
DEF_TRACE_SCOPE(areg_component_private_ServerList_registerServer);
DEF_TRACE_SCOPE(areg_component_private_ServerList_unregisterServer);

//////////////////////////////////////////////////////////////////////////
// Methods.
//////////////////////////////////////////////////////////////////////////
ServerList::MAPPOS ServerList::findServer(const ServerInfo& server) const
{
    return find(server);
}

ServerList::MAPPOS ServerList::findServer(const StubAddress& whichServer) const
{
    TRACE_SCOPE(areg_component_private_ServerList_findServer);
    TRACE_DBG("Search server based on server address [ %s ]", StubAddress::convAddressToPath(whichServer).getString());

    ServerInfo server(whichServer);
    return ServerList::find(server);
}

ServerList::MAPPOS ServerList::findServer(const ProxyAddress& whichClient) const
{
    TRACE_SCOPE(areg_component_private_ServerList_findServer);
    TRACE_DBG("Search server based on proxy address [ %s ]", ProxyAddress::convAddressToPath(whichClient).getString());

    ServerInfo server(whichClient);
    return ServerList::find(server);
}

const ServerInfo & ServerList::registerClient( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    TRACE_SCOPE(areg_component_private_ServerList_registerClient);

    std::pair<ServerListBase::MAPPOS, bool> added = addIfUnique(ServerInfo(whichClient), ClientList());
    TRACE_DBG("[ %s ] entry for client [ %s ]"
                , added.second ? "CREATED NEW" : "EXTRACTED EXISTING"
                , ProxyAddress::convAddressToPath(whichClient).getString());

    ServerListBase::MAPPOS pos = added.first;
    ASSERT(ServerListBase::isValidPosition(pos));

    out_client = pos->second.registerClient(whichClient, pos->first);
    TRACE_DBG("There are [ %d ] registered clients for service [ %s ]"
                , pos->second.getSize()
                , StubAddress::convAddressToPath(pos->first.getAddress()).getString());

    return pos->first;
}


ServerInfo ServerList::unregisterClient( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    TRACE_SCOPE(areg_component_private_ServerList_unregisterClient);

    ServerInfo result;
    ServerListBase::MAPPOS pos = findServer(whichClient);
    if (ServerListBase::isValidPosition(pos))
    {
        pos->second.unregisterClient(whichClient, out_client);
        result = pos->first;

        TRACE_DBG("Unregistered client [ %s ] from [ %s ] service [ %s ] with status [ %s ]. There are still [ %d ] registered clients"
                    , ProxyAddress::convAddressToPath(out_client.getAddress()).getString()
                    , pos->first.getAddress().isRemoteAddress() ? "REMOTE" : "LOCAL"
                    , StubAddress::convAddressToPath(pos->first.getAddress()).getString()
                    , NEService::getString(pos->first.getConnectionStatus())
                    , pos->second.getSize());

        if (pos->second.isEmpty())
        {
            const StubAddress & addrStub = pos->first.getAddress();
            if (addrStub.getSource() == NEService::SOURCE_UNKNOWN || addrStub.isRemoteAddress())
            {
                removePosition(pos);
            }
        }
    }
    else
    {
        TRACE_INFO("No service for client [ %s ], ignore unregister", ProxyAddress::convAddressToPath(whichClient).getString());
    }

    return result;
}

const ServerInfo & ServerList::registerServer( const StubAddress & addrStub, ClientList & out_clinetList )
{
    TRACE_SCOPE(areg_component_private_ServerList_registerServer);

    ASSERT(addrStub.isValid() );

    ServerInfo server(addrStub);
    std::pair<ServerListBase::MAPPOS, bool> added = addIfUnique(server, ClientList());
    TRACE_DBG("[ %s ] entry for server [ %s ]"
                , added.second ? "CREATED NEW" : "EXTRACTED EXISTING"
                , StubAddress::convAddressToPath(addrStub).getString());

    ServerListBase::MAPPOS pos = added.first;
    ASSERT(ServerListBase::isValidPosition(pos));

    ServerInfo& key = ServerListBase::keyAtPosition(pos);
    ClientList& value = ServerListBase::valueAtPosition(pos);

    key = server;
    key.setConnectionStatus( addrStub.getSource() != NEService::SOURCE_UNKNOWN ? NEService::eServiceConnection::ServiceConnected : NEService::eServiceConnection::ServicePending );
    value.serverAvailable(key, out_clinetList);

    TRACE_DBG("The [ %s ] service [ %s ] is with status [ %s ]. [ %d ] clients are going to be notified."
                    , addrStub.isRemoteAddress() ? "REMOTE" : "LOCAL"
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , NEService::getString(server.getConnectionStatus())
                    , out_clinetList.getSize());

    return key;
}

ServerInfo ServerList::unregisterServer( const StubAddress & whichServer, ClientList & out_clinetList )
{
    TRACE_SCOPE(areg_component_private_ServerList_unregisterServer);

    ServerInfo result(whichServer);
    ServerListBase::MAPPOS pos = find(result);

    if (ServerListBase::isValidPosition(pos))
    {
        ServerInfo& key = ServerListBase::keyAtPosition(pos);
        ClientList& value = ServerListBase::valueAtPosition(pos);

        result = key;
        value.serverUnavailable(out_clinetList);

        TRACE_INFO("Found and unregistered [ %s ] service [ %s ], [ %d ] clients are going to be notified, the list is [ %s ]"
                        , whichServer.isRemoteAddress() ? "REMOTE" : "LOCAL"
                        , StubAddress::convAddressToPath(whichServer).getString()
                        , out_clinetList.getSize()
                        , value.isEmpty() ? "EMPTY" : "NOT EMPTY");

        if ( value.isEmpty())
        {
            removePosition(pos);
        }
        else
        {
            key = static_cast<const ServiceAddress&>(whichServer);
        }
    }

    return result;
}

NEService::eServiceConnection ServerList::getServerState(const StubAddress & whichServer) const
{
    ServerListBase::MAPPOS pos = findServer(whichServer);
    return (ServerListBase::isValidPosition(pos) ? pos->first.getConnectionStatus() : NEService::eServiceConnection::ServiceConnectionUnknown);
}

const ClientList & ServerList::getClientList(const StubAddress & whichServer) const
{
    ServerListBase::MAPPOS pos = findServer(whichServer);
    ASSERT(ServerListBase::isValidPosition(pos));
    return pos->second;
}

bool ServerList::isServerRegistered(const StubAddress & server) const
{
    return (ServerListBase::isValidPosition(find(ServerInfo(server))));
}

const ServerInfo * ServerList::findClientServer(const ProxyAddress & whichClient) const
{
    ServerListBase::MAPPOS pos = findServer( whichClient );
    return ( ServerListBase::isValidPosition(pos) ? &(pos->first) : nullptr);
}
