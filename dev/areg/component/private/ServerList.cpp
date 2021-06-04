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

//////////////////////////////////////////////////////////////////////////
// ServerList class implementation
//////////////////////////////////////////////////////////////////////////

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
    MAPPOS pos  = findServer( whichClient );
    if ( pos == NULL )
    {
        pos = setAt( ServerInfo( whichClient ), ClientList( ), false );
    }
    ASSERT(pos != NULL);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    out_client = block->mValue.registerClient(whichClient, block->mKey);
    return block->mKey;
}


ServerInfo ServerList::unregisterClient( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    ServerInfo result;
    MAPPOS pos = findServer(whichClient);
    if ( pos != NULL )
    {
        ServiceBlock* block = static_cast<ServiceBlock *>(pos);
        block->mValue.unregisterClient(whichClient, out_client);
        result = block->mKey;

        if ( block->mValue.getSize() == 0 )
        {
            const StubAddress & addrStub = block->mKey.getAddress();
            if ( addrStub.getSource() == NEService::SOURCE_UNKNOWN || addrStub.isRemoteAddress() )
                removePosition( pos );
        }
    }
    return result;
}

const ServerInfo & ServerList::registerServer( const StubAddress & addrStub, ClientList & out_clinetList )
{
    ASSERT(addrStub.isValid() );

    ServerInfo server(addrStub);
    MAPPOS pos  = findServer(addrStub);
    if ( pos == NULL )
        pos = setAt( server, ClientList(), false);
    ASSERT(pos != NULL);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    block->mKey = server;
    block->mKey.setConnectionStatus( addrStub.getSource() != NEService::SOURCE_UNKNOWN ? NEService::ServiceConnected : NEService::ServicePending );
    block->mValue.serverAvailable(block->mKey, out_clinetList);
    return block->mKey;
}

ServerInfo ServerList::unregisterServer( const StubAddress & whichServer, ClientList & out_clinetList )
{
    ServerInfo result(whichServer);

    if (isEmpty() == false )
    {
        MAPPOS pos = find(result);
        ServiceBlock* block = static_cast<ServiceBlock *>(pos);
        if (block != NULL)
        {
            result = block->mKey;
            block->mValue.serverUnavailable(out_clinetList);
            if ( block->mValue.isEmpty())
            {
                removePosition(pos);
            }
            else
            {
                block->mKey = static_cast<const ServiceAddress &>(whichServer);
            }
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
