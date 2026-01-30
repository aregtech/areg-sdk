/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Event class implementation.
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
}

ProxyEvent::ProxyEvent( const IEInStream & stream )
    : StreamableEvent       ( stream )
    , mTargetProxyAddress   ( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class, Methods
//////////////////////////////////////////////////////////////////////////
void ProxyEvent::deliverEvent( void )
{
    if ( mTargetThread == nullptr )
    {
        Thread * thread = Thread::findThreadByName(mTargetProxyAddress.getThread());
        registerForThread( thread != nullptr ? RUNTIME_CAST(thread, DispatcherThread) : nullptr );
    }

    if ( mTargetThread != nullptr )
    {
        StreamableEvent::deliverEvent();
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
    : IEEventConsumer   ( )
    , mProxyAddress     ( proxy )
{
}

//////////////////////////////////////////////////////////////////////////
// IEProxyEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
inline void IEProxyEventConsumer::_localProcessResponseEvent(ResponseEvent & eventResponse)
{
    switch (eventResponse.getDataType() )
    {
    case NEService::eMessageDataType::RequestDataType:      // fall through
    case NEService::eMessageDataType::ResponseDataType:
        processResponseEvent(eventResponse);
        break;

    case NEService::eMessageDataType::AttributeDataType:
        processAttributeEvent(eventResponse);
        break;

    case NEService::eMessageDataType::ServiceDataType:      // fall through
    case NEService::eMessageDataType::UndefinedDataType:
        ASSERT(false);
        break;

    default:
        break;
    }
}

inline void IEProxyEventConsumer::_localProcessConnectEvent( ProxyConnectEvent & eventConnect )
{
    if ( eventConnect.getResponseId() == static_cast<unsigned int>(NEService::eFuncIdRange::ResponseServiceProviderConnection) )
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
    if ( proxyEvent != nullptr )
    {
        const ProxyAddress & addrProxy = proxyEvent->getTargetProxy();
        if ( static_cast<const ServiceAddress &>(addrProxy) == static_cast<const ServiceAddress &>(mProxyAddress) )
        {
            ProxyConnectEvent * eventConnect  = RUNTIME_CAST(&eventElem, ProxyConnectEvent);
            if ( eventConnect != nullptr )
            {
                _localProcessConnectEvent(*eventConnect);
            }
            else if ( addrProxy.getChannel() == mProxyAddress.getChannel() )
            {
                ResponseEvent * eventResponse = RUNTIME_CAST(&eventElem, ResponseEvent);
                if ( eventResponse != nullptr )
                {
                    _localProcessResponseEvent(*eventResponse);
                }
                else
                {
                    ServiceResponseEvent* eventServiceResponse = RUNTIME_CAST(&eventElem, ServiceResponseEvent);
                    if ( eventServiceResponse != nullptr )
                    {
                        processResponseEvent(*eventServiceResponse);
                    }
                    else
                    {
                            processProxyEvent(*proxyEvent);
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
