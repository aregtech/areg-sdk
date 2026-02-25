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
AREG_IMPLEMENT_RUNTIME_EVENT(ProxyEvent, StreamableEvent)

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ProxyEvent::ProxyEvent( const ProxyAddress & targetProxy, Event::EventType eventType )
    : StreamableEvent       (eventType)
    , mTargetProxyAddress   (targetProxy)
{
}

ProxyEvent::ProxyEvent( const InStream & stream )
    : StreamableEvent       ( stream )
    , mTargetProxyAddress   ( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class, Methods
//////////////////////////////////////////////////////////////////////////
void ProxyEvent::deliverEvent()
{
    if ( mTargetThread == nullptr )
    {
        areg::Thread * thread = areg::Thread::findThreadByName(mTargetProxyAddress.getThread());
        registerForThread( thread != nullptr ? AREG_RUNTIME_CAST(thread, DispatcherThread) : nullptr );
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

const InStream & ProxyEvent::readStream( const InStream & stream )
{
    StreamableEvent::readStream(stream);
    stream >> mTargetProxyAddress;
    return stream;
}

OutStream & ProxyEvent::writeStream( OutStream & stream ) const
{
    StreamableEvent::writeStream(stream);
    stream << mTargetProxyAddress;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// ProxyEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyEventConsumer::ProxyEventConsumer( const ProxyAddress & proxy )
    : EventConsumer   ( )
    , mProxyAddress     ( proxy )
{
}

//////////////////////////////////////////////////////////////////////////
// ProxyEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
inline void ProxyEventConsumer::_localProcessResponseEvent(ResponseEvent & eventResponse)
{
    switch (eventResponse.getDataType() )
    {
    case areg::MessageDataType::RequestData:      // fall through
    case areg::MessageDataType::ResponseData:
        processResponseEvent(eventResponse);
        break;

    case areg::MessageDataType::AttributeData:
        processAttributeEvent(eventResponse);
        break;

    case areg::MessageDataType::ServiceData:      // fall through
    case areg::MessageDataType::UndefinedData:
        ASSERT(false);
        break;

    default:
        break;
    }
}

inline void ProxyEventConsumer::_localProcessConnectEvent( ProxyConnectEvent & eventConnect )
{
    if ( eventConnect.getResponseId() == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) )
    {
        serviceConnectionUpdated( eventConnect.getStubAddress(), eventConnect.getTargetProxy().getChannel(), eventConnect.getConnectionStatus() );
    }
    else
    {
        ASSERT(false); // unknown  message
    }
}

void ProxyEventConsumer::startEventProcessing( Event & eventElem )
{
    ProxyEvent * proxyEvent = AREG_RUNTIME_CAST(&eventElem, ProxyEvent);
    if ( proxyEvent != nullptr )
    {
        const ProxyAddress & addrProxy = proxyEvent->getTargetProxy();
        if ( static_cast<const ServiceAddress &>(addrProxy) == static_cast<const ServiceAddress &>(mProxyAddress) )
        {
            ProxyConnectEvent * eventConnect  = AREG_RUNTIME_CAST(&eventElem, ProxyConnectEvent);
            if ( eventConnect != nullptr )
            {
                _localProcessConnectEvent(*eventConnect);
            }
            else if ( addrProxy.getChannel() == mProxyAddress.getChannel() )
            {
                ResponseEvent * eventResponse = AREG_RUNTIME_CAST(&eventElem, ResponseEvent);
                if ( eventResponse != nullptr )
                {
                    _localProcessResponseEvent(*eventResponse);
                }
                else
                {
                    ServiceResponseEvent* eventServiceResponse = AREG_RUNTIME_CAST(&eventElem, ServiceResponseEvent);
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
