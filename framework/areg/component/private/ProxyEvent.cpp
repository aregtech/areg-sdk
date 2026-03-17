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
namespace areg {

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
ProxyEvent::ProxyEvent( const ProxyAddress & targetProxy, areg::EventType eventType )
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
void ProxyEvent::deliver_event()
{
    if ( mTargetThread == nullptr )
    {
        Thread * thread = Thread::find_by_name(mTargetProxyAddress.thread());
        register_for_thread( thread != nullptr ? AREG_RUNTIME_CAST(thread, DispatcherThread) : nullptr );
    }

    if ( mTargetThread != nullptr )
    {
        StreamableEvent::deliver_event();
    }
    else
    {
        this->destroy();
    }
}

const InStream & ProxyEvent::read_stream( const InStream & stream )
{
    StreamableEvent::read_stream(stream);
    stream >> mTargetProxyAddress;
    return stream;
}

OutStream & ProxyEvent::write_stream( OutStream & stream ) const
{
    StreamableEvent::write_stream(stream);
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
inline void ProxyEventConsumer::_local_response(ResponseEvent & eventResponse)
{
    switch (eventResponse.data_type() )
    {
    case areg::MessageDataType::RequestData:      // fall through
    case areg::MessageDataType::ResponseData:
        process_response_event(eventResponse);
        break;

    case areg::MessageDataType::AttributeData:
        process_attribute_event(eventResponse);
        break;

    case areg::MessageDataType::ServiceData:      // fall through
    case areg::MessageDataType::UndefinedData:
        ASSERT(false);
        break;

    default:
        break;
    }
}

inline void ProxyEventConsumer::_local_connect( ProxyConnectEvent & eventConnect )
{
    if ( eventConnect.response_id() == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) )
    {
        service_connection_updated( eventConnect.stub_address(), eventConnect.target_proxy().channel(), eventConnect.connection_status() );
    }
    else
    {
        ASSERT(false); // unknown  message
    }
}

void ProxyEventConsumer::start_event_processing( Event & eventElem )
{
    ProxyEvent * proxyEvent = AREG_RUNTIME_CAST(&eventElem, ProxyEvent);
    if ( proxyEvent != nullptr )
    {
        const ProxyAddress & addrProxy = proxyEvent->target_proxy();
        if ( static_cast<const ServiceAddress &>(addrProxy) == static_cast<const ServiceAddress &>(mProxyAddress) )
        {
            ProxyConnectEvent * eventConnect  = AREG_RUNTIME_CAST(&eventElem, ProxyConnectEvent);
            if ( eventConnect != nullptr )
            {
                _local_connect(*eventConnect);
            }
            else if ( addrProxy.channel() == mProxyAddress.channel() )
            {
                ResponseEvent * eventResponse = AREG_RUNTIME_CAST(&eventElem, ResponseEvent);
                if ( eventResponse != nullptr )
                {
                    _local_response(*eventResponse);
                }
                else
                {
                    ServiceResponseEvent* eventServiceResponse = AREG_RUNTIME_CAST(&eventElem, ServiceResponseEvent);
                    if ( eventServiceResponse != nullptr )
                    {
                        process_response_event(*eventServiceResponse);
                    }
                    else
                    {
                            process_proxy_event(*proxyEvent);
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
        process_generic_event(eventElem);
    }
}

} // namespace areg
