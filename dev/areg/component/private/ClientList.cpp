/************************************************************************
 * \file        areg/component/private/ClientList.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client List implementation
 *
 ************************************************************************/
#include "areg/component/private/ClientList.hpp"

#include "areg/component/private/ServerInfo.hpp"
#include "areg/component/ProxyAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ClientList::ClientList( void )
    : TELinkedList<ClientInfo, const ClientInfo &, ClientListImpl> ( )
{
    ; // do nothing
}

ClientList::ClientList( const ClientList & src )
    : TELinkedList<ClientInfo, const ClientInfo &, ClientListImpl> (static_cast<const TELinkedList<ClientInfo, const ClientInfo &, ClientListImpl> &>(src))
{
    ; // do nothing
}

ClientList::~ClientList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const ClientList & ClientList::operator = ( const ClientList & src )
{
    static_cast<TELinkedList<ClientInfo, const ClientInfo &, ClientListImpl> &>(*this) = static_cast<const TELinkedList<ClientInfo, const ClientInfo &, ClientListImpl> &>(src);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool ClientList::existClient( const ProxyAddress & client ) const
{
    ClientInfo clientInfo(client);
    LISTPOS pos = firstPosition();
    for ( ; pos != NULL; pos = nextPosition(pos))
    {
        if ( equalValues(getAt(pos), clientInfo) )
            break;
    }
    return (pos != NULL ? true : false);
}

const ClientInfo & ClientList::getClient( const ProxyAddress & whichClient ) const
{
    ClientInfo clientInfo(whichClient);
    LISTPOS pos = firstPosition();
    for ( ; pos != NULL; pos = nextPosition(pos))
    {
        if ( equalValues(getAt(pos), clientInfo) )
            break;
    }
    return (pos != NULL ? getAt(pos) : ClientInfo::INVALID_CLIENT_INFO);
}

const ClientInfo & ClientList::registerClient( const ProxyAddress & whichClient, const ServerInfo & server )
{
    ClientInfo clientInfo(whichClient);
    LISTPOS pos = firstPosition();
    for ( ; pos != NULL; pos = nextPosition(pos))
    {
        if ( equalValues(getAt(pos), clientInfo) )
            break;
    }
    if (pos == NULL)
        pos = pushFirst( clientInfo );

    ClientInfo & client = getAt(pos);
    client.setTargetServer( server.getAddress() );
    client.setConnectionStatus( server.getConnectionStatus() );
    return client;
}

bool ClientList::unregisterClient( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    bool exist = false;
    ClientInfo clientInfo(whichClient);
    for ( LISTPOS pos = firstPosition(); pos != NULL; pos = nextPosition(pos) )
    {
        ClientInfo & client = getAt(pos);
        if ( equalValues(client, clientInfo) )
        {
            exist       = true;
            client.setTargetServer( StubAddress::INVALID_STUB_ADDRESS );
            client.setConnectionStatus( NEService::ServicePending );
            out_client  = client;
            removeAt(pos);
            break;
        }
    }
    return exist;
}

void ClientList::serverAvailable( const ServerInfo & whichServer, ClientList & out_clientList )
{
    NEService::eServiceConnection state = whichServer.getConnectionStatus();
    const StubAddress & addrStub = whichServer.getAddress();

    for ( LISTPOS pos = firstPosition(); pos != NULL; pos = nextPosition(pos))
    {
        ClientInfo & client = getAt(pos);
        client.setTargetServer(addrStub);
        client.setConnectionStatus( state );
        out_clientList.pushFirst(client);
    }
}

void ClientList::serverUnavailable( ClientList & out_clientList )
{
    for ( LISTPOS pos = firstPosition(); pos != NULL; )
    {
        ClientInfo & client = getAt( pos );
        client.setTargetServer( StubAddress::INVALID_STUB_ADDRESS );
        client.setConnectionStatus( NEService::ServicePending );
        out_clientList.pushFirst(client);
#if 1
        if ( client.getAddress().isRemoteAddress() )
        {
            pos = nextPosition(pos);
        }
        else
        {
            // remote client, remote it.
            // LISTPOS oldPos = pos;
            pos = nextPosition(pos);
            // RemoveAt(oldPos);
        }
#endif
    }
}
