/************************************************************************
 * \file        areg/src/component/private/CEProxyConnectEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Connection event class implementation.
 *
 ************************************************************************/
#include "areg/src/component/private/CEProxyConnectEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// CEProxyConnectEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CEProxyConnectEvent, CEServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEProxyConnectEvent::CEProxyConnectEvent( const CEProxyAddress & target
                                        , const CEStubAddress & implAddress
                                        , NEService::eServiceConnection connectStatus )
    : CEServiceResponseEvent( target, NEService::RESULT_DATA_OK, NEService::SI_SERVICE_CONNECTION_NOTIFY, CEEvent::EventProxyConnect )
    , mImplAddress          ( implAddress )
    , mConnectionStatus     ( connectStatus )
{
    ; // do nothing
}

CEProxyConnectEvent::CEProxyConnectEvent( const CEProxyAddress & target, const CEProxyConnectEvent & src )
    : CEServiceResponseEvent( target, static_cast<const CEServiceResponseEvent &>(src) )
    , mImplAddress          ( src.mImplAddress )
    , mConnectionStatus     ( src.mConnectionStatus )
{
    ; // do nothing
}

CEProxyConnectEvent::CEProxyConnectEvent(const IEInStream & stream)
    : CEServiceResponseEvent( stream )
    , mImplAddress          ( stream )
{
     stream >> mConnectionStatus;
}

CEProxyConnectEvent::~CEProxyConnectEvent( void )
{
    ; // do nothing
}

const IEInStream & CEProxyConnectEvent::ReadFromStream(const IEInStream & stream)
{
    CEServiceResponseEvent::ReadFromStream(stream);
    stream >> mImplAddress;
    stream >> mConnectionStatus;
    return stream;
}

IEOutStream & CEProxyConnectEvent::WriteToStream(IEOutStream & stream) const
{
    CEServiceResponseEvent::WriteToStream(stream);
    stream << mImplAddress;
    stream << mConnectionStatus;
    return stream;
}
