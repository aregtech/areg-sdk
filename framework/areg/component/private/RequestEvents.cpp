/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/RequestEvents.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
#include "areg/component/RequestEvents.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/private/ServiceManager.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// RequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(RequestEvent, ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RequestEvent::RequestEvent( const ProxyAddress & fromSource
                          , const StubAddress & toTarget
                          , uint32_t reqId
                          , Event::EventType eventType )
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, eventType)
    , mData(reqId, Event::is_external(eventType) ? EventDataStream::EventDataKind::External : EventDataStream::EventDataKind::Internal)
{
}

RequestEvent::RequestEvent( const EventDataStream & args
                          , const ProxyAddress & fromSource
                          , const StubAddress& toTarget
                          , uint32_t reqId
                          , Event::EventType eventType
                          , const String & name /*= String::empty_string()*/ )
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, eventType)
    , mData(reqId, args, name)
{
}

RequestEvent::RequestEvent( const InStream & stream )
    : ServiceRequestEvent( stream )
    , mData( stream )
{
}

const InStream & RequestEvent::read_stream(const InStream & stream)
{
    ServiceRequestEvent::read_stream(stream);
    stream >> mData;
    return stream;
}

OutStream & RequestEvent::write_stream(OutStream & stream) const
{
    ServiceRequestEvent::write_stream(stream);
    stream << mData;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(LocalRequestEvent, RequestEvent)

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalRequestEvent::LocalRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId )
    : RequestEvent(fromSource, toTarget, reqId, Event::EventType::EventLocalServiceRequest)
{
}

LocalRequestEvent::LocalRequestEvent( const EventDataStream & args
                                    , const ProxyAddress & fromSource
                                    , const StubAddress & toTarget
                                    , uint32_t reqId
                                    , const String & name /*= String::empty_string()*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, Event::EventType::EventLocalServiceRequest, name)
{
}

LocalRequestEvent::LocalRequestEvent( const InStream & stream )
    : RequestEvent ( stream)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(RemoteRequestEvent, RequestEvent)

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteRequestEvent::RemoteRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId )
    : RequestEvent(fromSource, toTarget, reqId, Event::EventType::EventRemoteServiceRequest)
{
}

RemoteRequestEvent::RemoteRequestEvent( const EventDataStream & args
                                      , const ProxyAddress & fromSource
                                      , const StubAddress & toTarget
                                      , uint32_t reqId
                                      , const String & name /*= String::empty_string()*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, Event::EventType::EventRemoteServiceRequest, name)
{
}

RemoteRequestEvent::RemoteRequestEvent( const InStream & stream )
    : RequestEvent ( stream)
{
    ASSERT(data().data_stream().is_external_stream());
}

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(NotifyRequestEvent, ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
NotifyRequestEvent::NotifyRequestEvent( const ProxyAddress & fromProxy
                                      , const StubAddress & toStub
                                      , uint32_t msgId
                                      , areg::RequestType reqType
                                      , Event::EventType eventType)
    : ServiceRequestEvent ( fromProxy, toStub, msgId, reqType, eventType)
{
}

NotifyRequestEvent::NotifyRequestEvent(const InStream & stream)
    : ServiceRequestEvent ( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(LocalNotifyRequestEvent, NotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalNotifyRequestEvent::LocalNotifyRequestEvent( const ProxyAddress & fromProxy
                                                , const StubAddress & toStub
                                                , uint32_t msgId
                                                , areg::RequestType reqType )
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, Event::EventType::EventLocalNotifyRequest)
{
}

LocalNotifyRequestEvent::LocalNotifyRequestEvent( const InStream & stream )
    : NotifyRequestEvent ( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(RemoteNotifyRequestEvent, NotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const ProxyAddress & fromProxy
                                                  , const StubAddress & toStub
                                                  , uint32_t msgId
                                                  , areg::RequestType reqType )
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, Event::EventType::EventRemoteNotifyRequest)
{
}

RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const InStream & stream )
    : NotifyRequestEvent ( stream )
{
}

} // namespace areg
