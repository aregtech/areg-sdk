/************************************************************************
 * \file        areg/component/private/ClientInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client Info implementation
 *
 ************************************************************************/
#include "areg/component/private/ClientInfo.hpp"
#include "areg/component/StubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Invalid client info object
 **/
const ClientInfo ClientInfo::INVALID_CLIENT_INFO;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

ClientInfo::ClientInfo( void )
    : mClientAddress( )
    , mClientState  ( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

ClientInfo::ClientInfo( const ProxyAddress & client )
    : mClientAddress( client )
    , mClientState  ( NEService::ServiceConnectionUnknown )
{
    setConnectionStatus( NEService::ServicePending);
}

ClientInfo::ClientInfo( const ClientInfo & src )
    : mClientAddress( src.mClientAddress )
    , mClientState  ( src.mClientState )
{
    ; // do nothing
}

ClientInfo::~ClientInfo( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

const ClientInfo & ClientInfo::operator = ( const ClientInfo & src )
{
    if (static_cast<const ClientInfo *>(this) != &src)
    {
        mClientAddress  = src.mClientAddress;
        mClientState    = src.mClientState;
    }
    return (*this);
}

const ClientInfo & ClientInfo::operator = ( const ProxyAddress & client )
{
    mClientAddress  = client;
    setConnectionStatus(NEService::ServicePending);
    return (*this);
}

inline bool ClientInfo::operator == (const ProxyAddress & client) const
{
    return mClientAddress == client;
}

bool ClientInfo::operator == (const ClientInfo & other) const
{
    return (this != &other ? mClientAddress == other.mClientAddress : true);
}

bool ClientInfo::operator == (const StubAddress & server) const
{
    return mClientAddress == server;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void ClientInfo::setConnectionStatus( NEService::eServiceConnection newConnection )
{
    if ( mClientAddress.getSource() != NEService::SOURCE_UNKNOWN )
    {
        mClientState = mClientAddress.getTarget() != NEService::TARGET_UNKNOWN ? newConnection : NEService::ServicePending;
    }
    else
    {
        mClientState = NEService::ServiceConnectionUnknown;
    }
}

ClientInfo::operator unsigned int (void) const
{
    const ServiceAddress & addrService = static_cast<const ServiceAddress &>(mClientAddress);
    return static_cast<unsigned int>( addrService );
}

void ClientInfo::setTargetServer(const StubAddress & addrStub)
{
    if ( mClientAddress.getSource() != NEService::SOURCE_UNKNOWN )
    {
        mClientAddress.setTarget( addrStub.getSource() );
    }
}
