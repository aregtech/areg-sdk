/************************************************************************
 * \file        areg/component/private/CEProxyEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Event class implementation.
 *
 ************************************************************************/
#include "areg/component/CEProxyEvent.hpp"
#include "areg/component/private/CEProxyConnectEvent.hpp"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/CEServiceResponseEvent.hpp"
#include "areg/component/CEProxyBase.hpp"
#include "areg/component/CEResponseEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// CEProxyEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyEvent class, runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CEProxyEvent, CEStreamableEvent)

//////////////////////////////////////////////////////////////////////////
// CEProxyEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEProxyEvent::CEProxyEvent( const CEProxyAddress & targetProxy, CEEvent::eEventType eventType )
    : CEStreamableEvent     (eventType)
    , mTargetProxyAddress   (targetProxy)
{
    ; // do nothing
}

CEProxyEvent::CEProxyEvent( const IEInStream & stream )
    : CEStreamableEvent     ( stream )
    , mTargetProxyAddress   ( stream )
{
    ; // do nothing
}

CEProxyEvent::~CEProxyEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEProxyEvent class, Methods
//////////////////////////////////////////////////////////////////////////
void CEProxyEvent::SendEvent( void )
{
    ASSERT(mTargetProxyAddress.IsValid());
    if ( mTargetThread == NULL )
    {
        CEThread * thread = CEThread::FindThreadByName(mTargetProxyAddress.GetThread());
        RegisterForThread( thread != NULL ? RUNTIME_CAST(thread, CEDispatcherThread) : NULL );
    }
    if ( mTargetThread != NULL )
        CEStreamableEvent::SendEvent();
    else
        this->Destroy();
}

const IEInStream & CEProxyEvent::ReadFromStream( const IEInStream & stream )
{
    CEStreamableEvent::ReadFromStream(stream);
    stream >> mTargetProxyAddress;
    return stream;
}

IEOutStream & CEProxyEvent::WriteToStream( IEOutStream & stream ) const
{
    CEStreamableEvent::WriteToStream(stream);
    stream << mTargetProxyAddress;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// IEProxyEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEProxyEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
IEProxyEventConsumer::IEProxyEventConsumer( const CEProxyAddress & proxy )
    : CEThreadEventConsumerBase ( )
    , mProxyAddress             ( proxy )
{
    ; // do nothing
}

IEProxyEventConsumer::~IEProxyEventConsumer( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// IEProxyEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
inline void IEProxyEventConsumer::processResponseEvent(CEResponseEvent & eventResponse)
{
    switch (eventResponse.GetDataType() )
    {
    case NEService::REQUEST_DATA_TYPE:
    case NEService::RESPONSE_DATA_TYPE:
        ProcessResponseEvent(eventResponse);
        break;

    case NEService::ATTRIBUTE_DATA_TYPE:
        ProcessAttributeEvent(eventResponse);
        break;

    default:
        break;
    }
}

inline void IEProxyEventConsumer::processConnectEvent( CEProxyConnectEvent & eventConnect )
{
    if ( eventConnect.GetResponseId() == NEService::SI_SERVICE_CONNECTION_NOTIFY )
    {
        ServiceConnectionUpdated( eventConnect.GetServerAddress(), eventConnect.GetTargetProxy().GetChannel(), eventConnect.GetConnectionStatus() );
    }
    else
    {
        ASSERT(false); // unknown  message
    }
}

void IEProxyEventConsumer::StartEventProcessing( CEEvent & eventElem )
{
    CEProxyEvent * proxyEvent = RUNTIME_CAST(&eventElem, CEProxyEvent);
    if ( proxyEvent != NULL )
    {
        const CEProxyAddress & addrProxy = proxyEvent->GetTargetProxy();
        if ( static_cast<const CEServiceAddress &>(addrProxy) == static_cast<const CEServiceAddress &>(mProxyAddress) )
        {
            CEProxyConnectEvent * eventConnect  = RUNTIME_CAST(&eventElem, CEProxyConnectEvent);
            if ( eventConnect != NULL )
            {
                processConnectEvent(*eventConnect);
            }
            else if ( addrProxy.GetChannel() == mProxyAddress.GetChannel() )
            {
                CEResponseEvent * eventResponse = RUNTIME_CAST(&eventElem, CEResponseEvent);
                if ( eventResponse != NULL )
                {
                    processResponseEvent(*eventResponse);
                }
                else
                {
                    CEServiceResponseEvent* eventServiceResponse = RUNTIME_CAST(&eventElem, CEServiceResponseEvent);
                    if ( eventServiceResponse != NULL )
                    {
                        ProcessResponseEvent(*eventServiceResponse);
                    }
                    else
                    {
                        CEProxyEvent* proxyEvent = RUNTIME_CAST(&eventElem, CEProxyEvent);
                        if (proxyEvent != NULL)
                            ProcessProxyEvent(*proxyEvent);
                        else
                            ProcessGenericEvent(eventElem);
                    }
                }
            }
        }
        else
        {
            ; // ignore, not relevant event
        }
    }
    else
    {
        ProcessGenericEvent(eventElem);
    }
}
