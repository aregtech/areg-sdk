/************************************************************************
 * \file        areg/src/component/private/CEClientList.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client List implementation
 *
 ************************************************************************/
#include "areg/src/component/private/CEClientList.hpp"

#include "areg/src/component/private/CEServerInfo.hpp"
#include "areg/src/component/CEProxyAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// CEClientList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEClientList::CEClientList( void )
    : TELinkedList<CEClientInfo, const CEClientInfo &, CEClientListImpl> ( )
{
    ; // do nothing
}

CEClientList::CEClientList( const CEClientList & src )
    : TELinkedList<CEClientInfo, const CEClientInfo &, CEClientListImpl> (static_cast<const TELinkedList<CEClientInfo, const CEClientInfo &, CEClientListImpl> &>(src))
{
    ; // do nothing
}

CEClientList::~CEClientList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const CEClientList & CEClientList::operator = ( const CEClientList & src )
{
    static_cast<TELinkedList<CEClientInfo, const CEClientInfo &, CEClientListImpl> &>(*this) = static_cast<const TELinkedList<CEClientInfo, const CEClientInfo &, CEClientListImpl> &>(src);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool CEClientList::ClientExist( const CEProxyAddress & client ) const
{
    CEClientInfo clientInfo(client);
    LISTPOS pos = GetHeadPosition();
    for ( ; pos != NULL; pos = GetNextPosition(pos))
    {
        if ( EqualValues(GetAt(pos), clientInfo) )
            break;
    }
    return (pos != NULL ? true : false);
}

const CEClientInfo & CEClientList::GetClient( const CEProxyAddress & whichClient ) const
{
    CEClientInfo clientInfo(whichClient);
    LISTPOS pos = GetHeadPosition();
    for ( ; pos != NULL; pos = GetNextPosition(pos))
    {
        if ( EqualValues(GetAt(pos), clientInfo) )
            break;
    }
    return (pos != NULL ? GetAt(pos) : CEClientInfo::INVALID_CLIENT_INFO);
}

const CEClientInfo & CEClientList::RegisterClient( const CEProxyAddress & whichClient, const CEServerInfo & server )
{
    CEClientInfo clientInfo(whichClient);
    LISTPOS pos = GetHeadPosition();
    for ( ; pos != NULL; pos = GetNextPosition(pos))
    {
        if ( EqualValues(GetAt(pos), clientInfo) )
            break;
    }
    if (pos == NULL)
        pos = AddTail( clientInfo );

    CEClientInfo & client = GetAt(pos);
    client.SetTargetServer( server.GetAddress() );
    client.SetConnectionStatus( server.GetConnectionState() );
    return client;
}

bool CEClientList::UnregisterClient( const CEProxyAddress & whichClient, CEClientInfo & out_client )
{
    bool exist = false;
    CEClientInfo clientInfo(whichClient);
    for ( LISTPOS pos = GetHeadPosition(); pos != NULL; pos = GetNextPosition(pos) )
    {
        CEClientInfo & client = GetAt(pos);
        if ( EqualValues(client, clientInfo) )
        {
            exist       = true;
            client.SetTargetServer( CEStubAddress::INVALID_STUB_ADDRESS );
            client.SetConnectionStatus( NEService::ServicePending );
            out_client  = client;
            RemoveAt(pos);
            break;
        }
    }
    return exist;
}

void CEClientList::SetServerAvailable( const CEServerInfo & whichServer, CEClientList & out_clientList )
{
    NEService::eServiceConnection state = whichServer.GetConnectionState();
    const CEStubAddress & addrStub = whichServer.GetAddress();

    for ( LISTPOS pos = GetHeadPosition(); pos != NULL; pos = GetNextPosition(pos))
    {
        CEClientInfo & client = GetAt(pos);
        client.SetTargetServer(addrStub);
        client.SetConnectionStatus( state );
        out_clientList.AddHead(client);
    }
}

void CEClientList::SetServerUnavailable( CEClientList & out_clientList )
{
    for ( LISTPOS pos = GetHeadPosition(); pos != NULL; )
    {
        CEClientInfo & client = GetAt( pos );
        client.SetTargetServer( CEStubAddress::INVALID_STUB_ADDRESS );
        client.SetConnectionStatus( NEService::ServicePending );
        out_clientList.AddTail(client);
#if 1
        if ( client.GetAddress().IsRemoteAddress() )
        {
            pos = GetNextPosition(pos);
        }
        else
        {
            // remote client, remote it.
            // LISTPOS oldPos = pos;
            pos = GetNextPosition(pos);
            // RemoveAt(oldPos);
        }
#endif
    }
}
