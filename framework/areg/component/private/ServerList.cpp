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
#include "areg/logging/GELog.h"

//////////////////////////////////////////////////////////////////////////
// ServerList class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_private_ServerList_findServer);
DEF_LOG_SCOPE(areg_component_private_ServerList_registerClient);
DEF_LOG_SCOPE(areg_component_private_ServerList_unregisterClient);
DEF_LOG_SCOPE(areg_component_private_ServerList_registerServer);
DEF_LOG_SCOPE(areg_component_private_ServerList_unregisterServer);

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // Methods.
    //////////////////////////////////////////////////////////////////////////
    ServerList::MAPPOS ServerList::findServer(const areg::ServerInfo& server) const
    {
        return find(server);
    }

    ServerList::MAPPOS ServerList::findServer(const areg::StubAddress& whichServer) const
    {
        LOG_SCOPE(areg_component_private_ServerList_findServer);
        LOG_DBG("Search server based on server address [ %s ]", areg::StubAddress::convAddressToPath(whichServer).getString());

        areg::ServerInfo server(whichServer);
        return ServerList::find(server);
    }

    ServerList::MAPPOS ServerList::findServer(const areg::ProxyAddress& whichClient) const
    {
        LOG_SCOPE(areg_component_private_ServerList_findServer);
        LOG_DBG("Search server based on proxy address [ %s ]", areg::ProxyAddress::convAddressToPath(whichClient).getString());

        areg::ServerInfo server(whichClient);
        return ServerList::find(server);
    }

    const areg::ServerInfo & ServerList::registerClient( const areg::ProxyAddress & whichClient, ClientInfo & out_client )
    {
        LOG_SCOPE(areg_component_private_ServerList_registerClient);

        std::pair<ServerListBase::MAPPOS, bool> added = addIfUnique(areg::ServerInfo(whichClient), areg::ClientList());
        LOG_DBG("[ %s ] entry for client [ %s ]"
                    , added.second ? "CREATED NEW" : "EXTRACTED EXISTING"
                    , areg::ProxyAddress::convAddressToPath(whichClient).getString());

        ServerListBase::MAPPOS pos = added.first;
        ASSERT(ServerListBase::isValidPosition(pos));

        out_client = pos->second.registerClient(whichClient, pos->first);
        LOG_DBG("There are [ %d ] registered clients for service [ %s ]"
                    , pos->second.getSize()
                    , areg::StubAddress::convAddressToPath(pos->first.getAddress()).getString());

        return pos->first;
    }


    areg::ServerInfo ServerList::unregisterClient( const areg::ProxyAddress & whichClient, ClientInfo & out_client )
    {
        LOG_SCOPE(areg_component_private_ServerList_unregisterClient);

        areg::ServerInfo result;
        ServerListBase::MAPPOS pos = findServer(whichClient);
        if (ServerListBase::isValidPosition(pos))
        {
            pos->second.unregisterClient(whichClient, out_client);
            result = pos->first;

            LOG_DBG("Unregistered client [ %s ] from [ %s ] service [ %s ] with status [ %s ]. There are still [ %d ] registered clients"
                        , areg::ProxyAddress::convAddressToPath(out_client.getAddress()).getString()
                        , pos->first.getAddress().isRemoteAddress() ? "REMOTE" : "LOCAL"
                        , areg::StubAddress::convAddressToPath(pos->first.getAddress()).getString()
                        , areg::getString(pos->first.getConnectionStatus())
                        , pos->second.getSize());

            if (pos->second.isEmpty())
            {
                const areg::StubAddress & addrStub = pos->first.getAddress();
                if (addrStub.getSource() == areg::SOURCE_UNKNOWN || addrStub.isRemoteAddress())
                {
                    removePosition(pos);
                }
            }
        }
        else
        {
            LOG_INFO("No service for client [ %s ], ignore unregister", areg::ProxyAddress::convAddressToPath(whichClient).getString());
        }

        return result;
    }

    const areg::ServerInfo & ServerList::registerServer( const areg::StubAddress & addrStub, areg::ClientList & out_clinetList )
    {
        LOG_SCOPE(areg_component_private_ServerList_registerServer);

        ASSERT(addrStub.isValid() );

        areg::ServerInfo server(addrStub);
        std::pair<ServerListBase::MAPPOS, bool> added = addIfUnique(server, areg::ClientList());
        LOG_DBG("[ %s ] entry for server [ %s ]"
                    , added.second ? "CREATED NEW" : "EXTRACTED EXISTING"
                    , areg::StubAddress::convAddressToPath(addrStub).getString());

        ServerListBase::MAPPOS pos = added.first;
        ASSERT(ServerListBase::isValidPosition(pos));

        areg::ServerInfo& key = ServerListBase::keyAtPosition(pos);
        areg::ClientList& value = ServerListBase::valueAtPosition(pos);

        key = server;
        key.setConnectionStatus( addrStub.getSource() != areg::SOURCE_UNKNOWN ? areg::ServiceConnectionState::Connected : areg::ServiceConnectionState::Pending );
        value.serverAvailable(key, out_clinetList);

        LOG_DBG("The [ %s ] service [ %s ] is with status [ %s ]. [ %d ] clients are going to be notified."
                        , addrStub.isRemoteAddress() ? "REMOTE" : "LOCAL"
                        , areg::StubAddress::convAddressToPath(addrStub).getString()
                        , areg::getString(server.getConnectionStatus())
                        , out_clinetList.getSize());

        return key;
    }

    areg::ServerInfo ServerList::unregisterServer( const areg::StubAddress & whichServer, areg::ClientList & out_clinetList )
    {
        LOG_SCOPE(areg_component_private_ServerList_unregisterServer);

        areg::ServerInfo result(whichServer);
        ServerListBase::MAPPOS pos = find(result);

        if (ServerListBase::isValidPosition(pos))
        {
            areg::ServerInfo& key = ServerListBase::keyAtPosition(pos);
            areg::ClientList& value = ServerListBase::valueAtPosition(pos);

            result = key;
            value.serverUnavailable(out_clinetList);

            LOG_INFO("Found and unregistered [ %s ] service [ %s ], [ %d ] clients are going to be notified, the list is [ %s ]"
                            , whichServer.isRemoteAddress() ? "REMOTE" : "LOCAL"
                            , areg::StubAddress::convAddressToPath(whichServer).getString()
                            , out_clinetList.getSize()
                            , value.isEmpty() ? "EMPTY" : "NOT EMPTY");

            if ( value.isEmpty())
            {
                removePosition(pos);
            }
            else
            {
                key = static_cast<const areg::ServiceAddress&>(whichServer);
            }
        }

        return result;
    }

    areg::ServiceConnectionState ServerList::getServerState(const areg::StubAddress & whichServer) const
    {
        ServerListBase::MAPPOS pos = findServer(whichServer);
        return (ServerListBase::isValidPosition(pos) ? pos->first.getConnectionStatus() : areg::ServiceConnectionState::Unknown);
    }

    const areg::ClientList & ServerList::getClientList(const areg::StubAddress & whichServer) const
    {
        ServerListBase::MAPPOS pos = findServer(whichServer);
        ASSERT(ServerListBase::isValidPosition(pos));
        return pos->second;
    }

    bool ServerList::isServerRegistered(const areg::StubAddress & server) const
    {
        return (ServerListBase::isValidPosition(find(areg::ServerInfo(server))));
    }

    const areg::ServerInfo * ServerList::findClientServer(const areg::ProxyAddress & whichClient) const
    {
        ServerListBase::MAPPOS pos = findServer( whichClient );
        return ( ServerListBase::isValidPosition(pos) ? &(pos->first) : nullptr);
    }
} // namespace areg
