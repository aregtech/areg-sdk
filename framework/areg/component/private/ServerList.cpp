/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServerList.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Server List class implementation
 *
 ************************************************************************/
#include "areg/component/private/ServerList.hpp"

#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/logging/areg_log.h"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServerList class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_private_ServerList_findServer);
DEF_LOG_SCOPE(areg_component_private_ServerList_registerClient);
DEF_LOG_SCOPE(areg_component_private_ServerList_unregisterClient);
DEF_LOG_SCOPE(areg_component_private_ServerList_registerServer);
DEF_LOG_SCOPE(areg_component_private_ServerList_unregisterServer);

//////////////////////////////////////////////////////////////////////////
// Methods.
//////////////////////////////////////////////////////////////////////////
ServerList::MAPPOS ServerList::find_server(const ServerInfo& server) const
{
    return find(server);
}

ServerList::MAPPOS ServerList::find_server(const StubAddress& whichServer) const
{
    LOG_SCOPE(areg_component_private_ServerList_findServer);
    LOG_DBG("Search server based on server address [ %s ]", StubAddress::to_path(whichServer).as_string());

    ServerInfo server(whichServer);
    return ServerList::find(server);
}

ServerList::MAPPOS ServerList::find_server(const ProxyAddress& whichClient) const
{
    LOG_SCOPE(areg_component_private_ServerList_findServer);
    LOG_DBG("Search server based on proxy address [ %s ]", ProxyAddress::to_path(whichClient).as_string());

    ServerInfo server(whichClient);
    return ServerList::find(server);
}

const ServerInfo & ServerList::register_client( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    LOG_SCOPE(areg_component_private_ServerList_registerClient);

    std::pair<ServerListBase::MAPPOS, bool> added = add_if_unique(ServerInfo(whichClient), ClientList());
    LOG_DBG("[ %s ] entry for client [ %s ]"
                , added.second ? "CREATED NEW" : "EXTRACTED EXISTING"
                , ProxyAddress::to_path(whichClient).as_string());

    ServerListBase::MAPPOS pos = added.first;
    ASSERT(ServerListBase::is_valid_position(pos));

    out_client = pos->second.register_client(whichClient, pos->first);
    LOG_DBG("There are [ %d ] registered clients for service [ %s ]"
                , pos->second.size()
                , StubAddress::to_path(pos->first.address()).as_string());

    return pos->first;
}


ServerInfo ServerList::unregister_client( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    LOG_SCOPE(areg_component_private_ServerList_unregisterClient);

    ServerInfo result;
    ServerListBase::MAPPOS pos = find_server(whichClient);
    if (ServerListBase::is_valid_position(pos))
    {
        pos->second.unregister_client(whichClient, out_client);
        result = pos->first;

        LOG_DBG("Unregistered client [ %s ] from [ %s ] service [ %s ] with status [ %s ]. There are still [ %d ] registered clients"
                    , ProxyAddress::to_path(out_client.address()).as_string()
                    , pos->first.address().is_remote_address() ? "REMOTE" : "LOCAL"
                    , StubAddress::to_path(pos->first.address()).as_string()
                    , areg::as_string(pos->first.connection_status())
                    , pos->second.size());

        if (pos->second.is_empty())
        {
            const StubAddress & addrStub = pos->first.address();
            if (addrStub.source() == areg::SOURCE_UNKNOWN || addrStub.is_remote_address())
            {
                remove_position(pos);
            }
        }
    }
    else
    {
        LOG_INFO("No service for client [ %s ], ignore unregister", ProxyAddress::to_path(whichClient).as_string());
    }

    return result;
}

const ServerInfo & ServerList::register_server( const StubAddress & addrStub, ClientList & out_clinetList )
{
    LOG_SCOPE(areg_component_private_ServerList_registerServer);

    ASSERT(addrStub.is_valid() );

    ServerInfo server(addrStub);
    std::pair<ServerListBase::MAPPOS, bool> added = add_if_unique(server, ClientList());
    LOG_DBG("[ %s ] entry for server [ %s ]"
                , added.second ? "CREATED NEW" : "EXTRACTED EXISTING"
                , StubAddress::to_path(addrStub).as_string());

    ServerListBase::MAPPOS pos = added.first;
    ASSERT(ServerListBase::is_valid_position(pos));

    ServerInfo& key = ServerListBase::key_at_position(pos);
    ClientList& value = ServerListBase::value_at_position(pos);

    key = server;
    key.set_connection_status( addrStub.source() != areg::SOURCE_UNKNOWN ? areg::ServiceConnectionState::Connected : areg::ServiceConnectionState::Pending );
    value.server_available(key, out_clinetList);

    LOG_DBG("The [ %s ] service [ %s ] is with status [ %s ]. [ %d ] clients are going to be notified."
                    , addrStub.is_remote_address() ? "REMOTE" : "LOCAL"
                    , StubAddress::to_path(addrStub).as_string()
                    , areg::as_string(server.connection_status())
                    , out_clinetList.size());

    return key;
}

ServerInfo ServerList::unregister_server( const StubAddress & whichServer, ClientList & out_clinetList )
{
    LOG_SCOPE(areg_component_private_ServerList_unregisterServer);

    ServerInfo result(whichServer);
    ServerListBase::MAPPOS pos = find(result);

    if (ServerListBase::is_valid_position(pos))
    {
        ServerInfo& key = ServerListBase::key_at_position(pos);
        ClientList& value = ServerListBase::value_at_position(pos);

        result = key;
        value.server_unavailable(out_clinetList);

        LOG_INFO("Found and unregistered [ %s ] service [ %s ], [ %d ] clients are going to be notified, the list is [ %s ]"
                        , whichServer.is_remote_address() ? "REMOTE" : "LOCAL"
                        , StubAddress::to_path(whichServer).as_string()
                        , out_clinetList.size()
                        , value.is_empty() ? "EMPTY" : "NOT EMPTY");

        if ( value.is_empty())
        {
            remove_position(pos);
        }
        else
        {
            key = static_cast<const ServiceAddress&>(whichServer);
        }
    }

    return result;
}

areg::ServiceConnectionState ServerList::server_state(const StubAddress & whichServer) const
{
    ServerListBase::MAPPOS pos = find_server(whichServer);
    return (ServerListBase::is_valid_position(pos) ? pos->first.connection_status() : areg::ServiceConnectionState::Unknown);
}

const ClientList & ServerList::client_list(const StubAddress & whichServer) const
{
    ServerListBase::MAPPOS pos = find_server(whichServer);
    ASSERT(ServerListBase::is_valid_position(pos));
    return pos->second;
}

bool ServerList::is_server_registered(const StubAddress & server) const
{
    return (ServerListBase::is_valid_position(find(ServerInfo(server))));
}

const ServerInfo * ServerList::find_client_server(const ProxyAddress & whichClient) const
{
    ServerListBase::MAPPOS pos = find_server( whichClient );
    return ( ServerListBase::is_valid_position(pos) ? &(pos->first) : nullptr);
}

} // namespace areg
