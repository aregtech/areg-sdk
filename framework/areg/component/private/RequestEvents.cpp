/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/RequestEvents.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
#include "areg/component/RequestEvents.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/private/ServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// RequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(RequestEvent, ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RequestEvent::RequestEvent( const ProxyAddress & fromSource
                          , const StubAddress & toTarget
                          , unsigned int reqId
                          , Event::eEventType eventType )
    : ServiceRequestEvent(fromSource, toTarget, reqId, NEService::eRequestType::CallFunction, eventType)
    , mData(reqId, Event::isExternal(eventType) ? EventDataStream::eEventData::EventDataExternal : EventDataStream::eEventData::EventDataInternal)
{
}

RequestEvent::RequestEvent( const EventDataStream & args
                          , const ProxyAddress & fromSource
                          , const StubAddress& toTarget
                          , unsigned int reqId
                          , Event::eEventType eventType
                          , const char* name /*= nullptr*/ )
    : ServiceRequestEvent(fromSource, toTarget, reqId, NEService::eRequestType::CallFunction, eventType)
    , mData(reqId, args, name)
{
}

RequestEvent::RequestEvent( const IEInStream & stream )
    : ServiceRequestEvent( stream )
    , mData( stream )
{
}

const IEInStream & RequestEvent::readStream(const IEInStream & stream)
{
    ServiceRequestEvent::readStream(stream);
    stream >> mData;
    return stream;
}

IEOutStream & RequestEvent::writeStream(IEOutStream & stream) const
{
    ServiceRequestEvent::writeStream(stream);
    stream << mData;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(LocalRequestEvent, RequestEvent)

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalRequestEvent::LocalRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RequestEvent(fromSource, toTarget, reqId, Event::eEventType::EventLocalServiceRequest)
{
}

LocalRequestEvent::LocalRequestEvent( const EventDataStream & args
                                    , const ProxyAddress & fromSource
                                    , const StubAddress & toTarget
                                    , unsigned int reqId
                                    , const char* name /*= nullptr*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, Event::eEventType::EventLocalServiceRequest, name)
{
}

LocalRequestEvent::LocalRequestEvent( const IEInStream & stream )
    : RequestEvent ( stream)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(RemoteRequestEvent, RequestEvent)

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteRequestEvent::RemoteRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RequestEvent(fromSource, toTarget, reqId, Event::eEventType::EventRemoteServiceRequest)
{
}

RemoteRequestEvent::RemoteRequestEvent( const EventDataStream & args
                                          , const ProxyAddress & fromSource
                                          , const StubAddress & toTarget
                                          , unsigned int reqId
                                          , const char* name /*= nullptr*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, Event::eEventType::EventRemoteServiceRequest, name)
{
}

RemoteRequestEvent::RemoteRequestEvent( const IEInStream & stream )
    : RequestEvent ( stream)
{
    ASSERT(getData().getDataStream().isExternalDataStream());
}

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(NotifyRequestEvent, ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
NotifyRequestEvent::NotifyRequestEvent( const ProxyAddress & fromProxy
                                          , const StubAddress & toStub
                                          , unsigned int msgId
                                          , NEService::eRequestType reqType
                                          , Event::eEventType eventType)
    : ServiceRequestEvent ( fromProxy, toStub, msgId, reqType, eventType)
{
}

NotifyRequestEvent::NotifyRequestEvent(const IEInStream & stream)
    : ServiceRequestEvent ( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(LocalNotifyRequestEvent, NotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalNotifyRequestEvent::LocalNotifyRequestEvent( const ProxyAddress & fromProxy
                                                    , const StubAddress & toStub
                                                    , unsigned int msgId
                                                    , NEService::eRequestType reqType )
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, Event::eEventType::EventLocalNotifyRequest)
{
}

LocalNotifyRequestEvent::LocalNotifyRequestEvent( const IEInStream & stream )
    : NotifyRequestEvent ( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(RemoteNotifyRequestEvent, NotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const ProxyAddress & fromProxy
                                                      , const StubAddress & toStub
                                                      , unsigned int msgId
                                                      , NEService::eRequestType reqType )
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, Event::eEventType::EventRemoteNotifyRequest)
{
}

RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const IEInStream & stream )
    : NotifyRequestEvent ( stream )
{
}
