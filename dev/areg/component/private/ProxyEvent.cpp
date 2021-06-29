/************************************************************************
 * \file        areg/component/private/ProxyEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Event class implementation.
 *
 ************************************************************************/
#include "areg/component/ProxyEvent.hpp"
#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ResponseEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class, runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(ProxyEvent, StreamableEvent)

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ProxyEvent::ProxyEvent( const ProxyAddress & targetProxy, Event::eEventType eventType )
    : StreamableEvent       (eventType)
    , mTargetProxyAddress   (targetProxy)
{
    ; // do nothing
}

ProxyEvent::ProxyEvent( const IEInStream & stream )
    : StreamableEvent       ( stream )
    , mTargetProxyAddress   ( stream )
{
    ; // do nothing
}

ProxyEvent::~ProxyEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class, Methods
//////////////////////////////////////////////////////////////////////////
void ProxyEvent::sendEvent( void )
{
    if ( mTargetThread == NULL )
    {
        Thread * thread = Thread::findThreadByName(mTargetProxyAddress.getThread());
        registerForThread( thread != NULL ? RUNTIME_CAST(thread, DispatcherThread) : NULL );
    }

    if ( mTargetThread != NULL )
    {
        StreamableEvent::sendEvent();
    }
    else
    {
        this->destroy();
    }
}

const IEInStream & ProxyEvent::readStream( const IEInStream & stream )
{
    StreamableEvent::readStream(stream);
    stream >> mTargetProxyAddress;
    return stream;
}

IEOutStream & ProxyEvent::writeStream( IEOutStream & stream ) const
{
    StreamableEvent::writeStream(stream);
    stream << mTargetProxyAddress;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// IEProxyEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEProxyEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
IEProxyEventConsumer::IEProxyEventConsumer( const ProxyAddress & proxy )
    : ThreadEventConsumerBase   ( )
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
inline void IEProxyEventConsumer::localProcessResponseEvent(ResponseEvent & eventResponse)
{
    switch (eventResponse.getDataType() )
    {
    case NEService::REQUEST_DATA_TYPE:
    case NEService::RESPONSE_DATA_TYPE:
        processResponseEvent(eventResponse);
        break;

    case NEService::ATTRIBUTE_DATA_TYPE:
        processAttributeEvent(eventResponse);
        break;

    default:
        break;
    }
}

inline void IEProxyEventConsumer::localProcessConnectEvent( ProxyConnectEvent & eventConnect )
{
    if ( eventConnect.getResponseId() == NEService::SI_SERVICE_CONNECTION_NOTIFY )
    {
        serviceConnectionUpdated( eventConnect.getStubAddress(), eventConnect.getTargetProxy().getChannel(), eventConnect.getConnectionStatus() );
    }
    else
    {
        ASSERT(false); // unknown  message
    }
}

void IEProxyEventConsumer::startEventProcessing( Event & eventElem )
{
    ProxyEvent * proxyEvent = RUNTIME_CAST(&eventElem, ProxyEvent);
    if ( proxyEvent != NULL )
    {
        const ProxyAddress & addrProxy = proxyEvent->getTargetProxy();
        if ( static_cast<const ServiceAddress &>(addrProxy) == static_cast<const ServiceAddress &>(mProxyAddress) )
        {
            ProxyConnectEvent * eventConnect  = RUNTIME_CAST(&eventElem, ProxyConnectEvent);
            if ( eventConnect != NULL )
            {
                localProcessConnectEvent(*eventConnect);
            }
            else if ( addrProxy.getChannel() == mProxyAddress.getChannel() )
            {
                ResponseEvent * eventResponse = RUNTIME_CAST(&eventElem, ResponseEvent);
                if ( eventResponse != NULL )
                {
                    localProcessResponseEvent(*eventResponse);
                }
                else
                {
                    ServiceResponseEvent* eventServiceResponse = RUNTIME_CAST(&eventElem, ServiceResponseEvent);
                    if ( eventServiceResponse != NULL )
                    {
                        processResponseEvent(*eventServiceResponse);
                    }
                    else
                    {
                        ProxyEvent* proxyEvent = RUNTIME_CAST(&eventElem, ProxyEvent);
                        if (proxyEvent != NULL)
                            processProxyEvent(*proxyEvent);
                        else
                            processGenericEvent(eventElem);
                    }
                }
            }
        }
        else
        {
            // ignore, not relevant for target proxy event
        }
    }
    else
    {
        processGenericEvent(eventElem);
    }
}
