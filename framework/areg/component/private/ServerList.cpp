/************************************************************************
 * \file        areg/component/private/ServerList.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ServerList::ServerList( void )
    : TEHashMap<ServerInfo, ClientList, const ServerInfo &, const ClientList &, ServerListImpl>( )
{
    ; // do nothing
}

ServerList::~ServerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods.
//////////////////////////////////////////////////////////////////////////
MAPPOS ServerList::findServer(const ServerInfo & server) const
{
    return find(server);
}

MAPPOS ServerList::findServer( const StubAddress & whichServer ) const
{
    TRACE_SCOPE(areg_component_private_ServerList_findServer);
    TRACE_DBG("Search server [ %s ]", StubAddress::convAddressToPath(whichServer).getString());

    ServerInfo server(whichServer);

    const unsigned int hash = getHashKey(server);
    ServiceBlock** block    = &mHashTable[hash % mHashTableSize];

    for ( ; *block != NULL; block = &(*block)->mNext)
    {
        if ( hash == (*block)->mHash )
        {
            if ( static_cast<const ServiceAddress &>(whichServer) == static_cast<const ServiceAddress &>((*block)->mKey.getAddress()) )
            {
                break;
            }
        }
    }

    TRACE_DBG("[ %s ] server [ %s ] in the list", (*block ) != NULL ? "FOUND" : "DIDNOT FIND", StubAddress::convAddressToPath(whichServer).getString());

    return (*block);
}

MAPPOS ServerList::findServer( const ProxyAddress & whichClient ) const
{
    ServerInfo server(whichClient);

    const unsigned int hash = getHashKey(server);
    ServiceBlock** block    = &mHashTable[hash % mHashTableSize];

    for ( ; *block != NULL; block = &(*block)->mNext)
    {
        if ( hash == (*block)->mHash )
        {
            if ( static_cast<const ServiceAddress &>(whichClient) == static_cast<const ServiceAddress &>((*block)->mKey.getAddress()) )
            {
                break;
            }
        }
    }
    
    return (*block);
}

const ServerInfo & ServerList::registerClient( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    TRACE_SCOPE(areg_component_private_ServerList_registerClient);

    MAPPOS pos  = findServer( whichClient );
    if ( pos == NULL )
    {
        TRACE_DBG("No service for client [ %s ], crate new entry.", ProxyAddress::convAddressToPath(whichClient).getString());
        pos = setAt( ServerInfo( whichClient ), ClientList( ), false );
    }

    ASSERT(pos != NULL);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    out_client = block->mValue.registerClient(whichClient, block->mKey);
    TRACE_DBG("There are [ %d ] registered clients for service [ %s ]", block->mValue.getSize(), StubAddress::convAddressToPath(block->mKey.getAddress()).getString());

    return block->mKey;
}


ServerInfo ServerList::unregisterClient( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    TRACE_SCOPE(areg_component_private_ServerList_unregisterClient);

    ServerInfo result;
    MAPPOS pos = findServer(whichClient);
    if ( pos != NULL )
    {
        ServiceBlock* block = static_cast<ServiceBlock *>(pos);
        block->mValue.unregisterClient(whichClient, out_client);
        result = block->mKey;

        TRACE_DBG("Unregistered client [ %s ] from [ %s ] service [ %s ] with status [ %s ]. There are still [ %d ] registered clients"
                    , ProxyAddress::convAddressToPath(out_client.getAddress()).getString()
                    , block->mKey.getAddress().isRemoteAddress() ? "REMOTE" : "LOCAL"
                    , StubAddress::convAddressToPath(block->mKey.getAddress()).getString()
                    , NEService::getString(block->mKey.getConnectionStatus())
                    , block->mValue.getSize());

        if ( block->mValue.getSize() == 0 )
        {
            const StubAddress & addrStub = block->mKey.getAddress();
            if ( addrStub.getSource() == NEService::SOURCE_UNKNOWN || addrStub.isRemoteAddress() )
                removePosition( pos );
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
    MAPPOS pos  = findServer(addrStub);
    if ( pos == NULL )
    {
        TRACE_DBG("There are still no clients for service [ %s ], create new entry", StubAddress::convAddressToPath(addrStub).getString());
        pos = setAt( server, ClientList(), false);
    }

    ASSERT(pos != NULL);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    block->mKey = server;
    block->mKey.setConnectionStatus( addrStub.getSource() != NEService::SOURCE_UNKNOWN ? NEService::ServiceConnected : NEService::ServicePending );
    block->mValue.serverAvailable(block->mKey, out_clinetList);

    TRACE_DBG("The [ %s ] service [ %s ] is with status [ %s ]. [ %d ] clients are going to be notified."
                    , addrStub.isRemoteAddress() ? "REMOTE" : "LOCAL"
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , NEService::getString(server.getConnectionStatus())
                    , out_clinetList.getSize());

    return block->mKey;
}

ServerInfo ServerList::unregisterServer( const StubAddress & whichServer, ClientList & out_clinetList )
{
    TRACE_SCOPE(areg_component_private_ServerList_unregisterServer);

    ServerInfo result(whichServer);

    MAPPOS pos = find(result);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    if (block != NULL)
    {
        result = block->mKey;
        block->mValue.serverUnavailable(out_clinetList);

        TRACE_INFO("Found and unregistered [ %s ] service [ %s ], [ %d ] clients are going to be notified, the list is [ %s ]"
                        , whichServer.isRemoteAddress() ? "REMOTE" : "LOCAL"
                        , StubAddress::convAddressToPath(whichServer).getString()
                        , out_clinetList.getSize()
                        , block->mValue.isEmpty() ? "EMPTY" : "NOT EMPTY");

        if ( block->mValue.isEmpty())
        {
            removePosition(pos);
        }
        else
        {
            block->mKey = static_cast<const ServiceAddress &>(whichServer);
        }
    }

    return result;
}

NEService::eServiceConnection ServerList::getServerState(const StubAddress & whichServer) const
{
    MAPPOS pos = findServer(whichServer);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    return (block != NULL ? block->mKey.getConnectionStatus() : NEService::ServiceConnectionUnknown);
}

const ClientList & ServerList::getClientList(const StubAddress & whichServer) const
{
    MAPPOS pos = findServer(whichServer);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    ASSERT(block != NULL);
    return block->mValue;
}

bool ServerList::isServerRegistered(const StubAddress & server) const
{
    return find(ServerInfo(server)) != NULL;
}

const ServerInfo * ServerList::findClientServer(const ProxyAddress & whichClient) const
{
    MAPPOS pos = findServer( whichClient );
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    return ( block != NULL ? &block->mKey : NULL );
}
