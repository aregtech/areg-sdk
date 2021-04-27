/************************************************************************
 * \file        areg/component/private/CEServerList.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Server List class implementation
 *
 ************************************************************************/
#include "areg/component/private/CEServerList.hpp"

#include "areg/component/CEStubAddress.hpp"
#include "areg/component/CEProxyAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// CEServerList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEServerList::CEServerList( void )
    : TEHashMap<CEServerInfo, CEClientList, const CEServerInfo &, const CEClientList &, CEServerListImpl>( )
{
    ; // do nothing
}

CEServerList::~CEServerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods.
//////////////////////////////////////////////////////////////////////////
MAPPOS CEServerList::FindServer(const CEServerInfo & server) const
{
    return Find(server);
}

MAPPOS CEServerList::FindServer( const CEStubAddress & whichServer ) const
{
    CEServerInfo server(whichServer);

    const unsigned int hash = GetHashKey(server);
    ServiceBlock** block    = &mHashTable[hash % mHashTableSize];

    for ( ; *block != NULL; block = &(*block)->mNext)
    {
        if ( hash == (*block)->mHash && (*block)->mKey == whichServer)
            break;
    }
    return (*block);
}

MAPPOS CEServerList::FindServer( const CEProxyAddress & whichClient ) const
{
    CEServerInfo server(whichClient);

    const unsigned int hash = GetHashKey(server);
    ServiceBlock** block    = &mHashTable[hash % mHashTableSize];

    for ( ; *block != NULL; block = &(*block)->mNext)
    {
        if ( hash == (*block)->mHash && (*block)->mKey == whichClient)
            break;
    }
    
    return (*block);
}

const CEServerInfo & CEServerList::RegisterClient( const CEProxyAddress & whichClient, CEClientInfo & out_client )
{
    MAPPOS pos  = FindServer( whichClient );
    if ( pos == NULL )
    {
        pos = SetKey( CEServerInfo( whichClient ), CEClientList( ), false );
    }
    ASSERT(pos != NULL);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    out_client = block->mValue.RegisterClient(whichClient, block->mKey);
    return block->mKey;
}


CEServerInfo CEServerList::UnregisterClient( const CEProxyAddress & whichClient, CEClientInfo & out_client )
{
    CEServerInfo result;
    MAPPOS pos = FindServer(whichClient);
    if ( pos != NULL )
    {
        ServiceBlock* block = static_cast<ServiceBlock *>(pos);
        block->mValue.UnregisterClient(whichClient, out_client);
        result = block->mKey;

        if ( block->mValue.GetSize() == 0 )
        {
            const CEStubAddress & addrStub = block->mKey.GetAddress();
            if ( addrStub.GetSource() == NEService::SOURCE_UNKNOWN || addrStub.IsRemoteAddress() )
                RemoveAt( pos );
        }
    }
    return result;
}

const CEServerInfo & CEServerList::RegisterServer( const CEStubAddress & addrStub, CEClientList & out_clinetList )
{
    ASSERT(addrStub.IsValid() );

    CEServerInfo server(addrStub);
    MAPPOS pos  = FindServer(addrStub);
    if ( pos == NULL )
        pos = SetKey( server, CEClientList(), false);
    ASSERT(pos != NULL);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    block->mKey = server;
    block->mKey.SetConnectionStatus( addrStub.GetSource() != NEService::SOURCE_UNKNOWN ? NEService::ServiceConnected : NEService::ServicePending );
    block->mValue.SetServerAvailable(block->mKey, out_clinetList);
    return block->mKey;
}

CEServerInfo CEServerList::UnregisterServer( const CEStubAddress & whichServer, CEClientList & out_clinetList )
{
    CEServerInfo result(whichServer);

    MAPPOS pos = Find(result);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    if (block != NULL)
    {
        result = block->mKey;
        block->mValue.SetServerUnavailable(out_clinetList);
        if ( block->mValue.IsEmpty())
            RemoveAt(pos);
        else
            block->mKey = static_cast<const CEServiceAddress &>(whichServer);
    }
    return result;
}

NEService::eServiceConnection CEServerList::GetServerState(const CEStubAddress & whichServer) const
{
    MAPPOS pos = FindServer(whichServer);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    return (block != NULL ? block->mKey.GetConnectionState() : NEService::ServiceConnectionUnknown);
}

const CEClientList & CEServerList::GetClientList(const CEStubAddress & whichServer) const
{
    MAPPOS pos = FindServer(whichServer);
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    ASSERT(block != NULL);
    return block->mValue;
}

bool CEServerList::IsServerRegistered(const CEStubAddress & server) const
{
    return Find(CEServerInfo(server)) != NULL;
}

const CEServerInfo * CEServerList::FindClientServer(const CEProxyAddress & whichClient) const
{
    MAPPOS pos = FindServer( whichClient );
    ServiceBlock* block = static_cast<ServiceBlock *>(pos);
    return ( block != NULL ? &block->mKey : NULL );
}
