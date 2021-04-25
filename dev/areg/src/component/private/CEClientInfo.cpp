/************************************************************************
 * \file        areg/src/component/private/CEClientInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client Info implementation
 *
 ************************************************************************/
#include "areg/src/component/private/CEClientInfo.hpp"
#include "areg/src/component/CEStubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// CEClientInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Invalid client info object
 **/
const CEClientInfo CEClientInfo::INVALID_CLIENT_INFO;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEClientInfo::CEClientInfo( void )
    : mClientAddress( )
    , mClientState  ( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

CEClientInfo::CEClientInfo( const CEProxyAddress & client )
    : mClientAddress( client )
    , mClientState  ( NEService::ServiceConnectionUnknown )
{
    SetConnectionStatus( NEService::ServicePending);
}

CEClientInfo::CEClientInfo( const CEClientInfo & src )
    : mClientAddress( src.mClientAddress )
    , mClientState  ( src.mClientState )
{
    ; // do nothing
}

CEClientInfo::~CEClientInfo( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

const CEClientInfo & CEClientInfo::operator = ( const CEClientInfo & src )
{
    if (static_cast<const CEClientInfo *>(this) != &src)
    {
        mClientAddress  = src.mClientAddress;
        mClientState    = src.mClientState;
    }
    return (*this);
}

const CEClientInfo & CEClientInfo::operator = ( const CEProxyAddress & client )
{
    mClientAddress  = client;
    SetConnectionStatus(NEService::ServicePending);
    return (*this);
}

inline bool CEClientInfo::operator == (const CEProxyAddress & client) const
{
    return mClientAddress == client;
}

bool CEClientInfo::operator == (const CEClientInfo & other) const
{
    return (this != &other ? mClientAddress == other.mClientAddress : true);
}

bool CEClientInfo::operator == (const CEStubAddress & server) const
{
    return mClientAddress == server;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void CEClientInfo::SetConnectionStatus( NEService::eServiceConnection newConnection )
{
    if ( mClientAddress.GetSource() != NEService::SOURCE_UNKNOWN )
    {
        mClientState = mClientAddress.GetTarget() != NEService::TARGET_UNKNOWN ? newConnection : NEService::ServicePending;
    }
    else
    {
        mClientState = NEService::ServiceConnectionUnknown;
    }
}

CEClientInfo::operator unsigned int(void) const
{
    return static_cast<unsigned int>(mClientAddress);
}

void CEClientInfo::SetTargetServer(const CEStubAddress & addrStub)
{
    if ( mClientAddress.GetSource() != NEService::SOURCE_UNKNOWN )
    {
        mClientAddress.SetTarget( addrStub.GetSource() );
    }
}
