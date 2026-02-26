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

//////////////////////////////////////////////////////////////////////////
// RequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(RequestEvent, areg::ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RequestEvent::RequestEvent( const ProxyAddress & fromSource
                          , const areg::StubAddress & toTarget
                          , uint32_t reqId
                          , areg::Event::EventType eventType )
    : areg::ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, eventType)
    , mData(reqId, areg::Event::isExternal(eventType) ? areg::EventDataStream::EventDataKind::External : areg::EventDataStream::EventDataKind::Internal)
{
}

RequestEvent::RequestEvent( const areg::EventDataStream & args
                          , const ProxyAddress & fromSource
                          , const areg::StubAddress& toTarget
                          , uint32_t reqId
                          , areg::Event::EventType eventType
                          , const areg::String & name /*= areg::String::getEmptyString()*/ )
    : areg::ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, eventType)
    , mData(reqId, args, name)
{
}

RequestEvent::RequestEvent( const areg::InStream & stream )
    : areg::ServiceRequestEvent( stream )
    , mData( stream )
{
}

const areg::InStream & RequestEvent::readStream(const areg::InStream & stream)
{
    areg::ServiceRequestEvent::readStream(stream);
    stream >> mData;
    return stream;
}

areg::OutStream & RequestEvent::writeStream(areg::OutStream & stream) const
{
    areg::ServiceRequestEvent::writeStream(stream);
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
LocalRequestEvent::LocalRequestEvent( const ProxyAddress & fromSource, const areg::StubAddress & toTarget, uint32_t reqId )
    : RequestEvent(fromSource, toTarget, reqId, areg::Event::EventType::EventLocalServiceRequest)
{
}

LocalRequestEvent::LocalRequestEvent( const areg::EventDataStream & args
                                    , const ProxyAddress & fromSource
                                    , const areg::StubAddress & toTarget
                                    , uint32_t reqId
                                    , const areg::String & name /*= areg::String::getEmptyString()*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, areg::Event::EventType::EventLocalServiceRequest, name)
{
}

LocalRequestEvent::LocalRequestEvent( const areg::InStream & stream )
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
RemoteRequestEvent::RemoteRequestEvent( const ProxyAddress & fromSource, const areg::StubAddress & toTarget, uint32_t reqId )
    : RequestEvent(fromSource, toTarget, reqId, areg::Event::EventType::EventRemoteServiceRequest)
{
}

RemoteRequestEvent::RemoteRequestEvent( const areg::EventDataStream & args
                                      , const ProxyAddress & fromSource
                                      , const areg::StubAddress & toTarget
                                      , uint32_t reqId
                                      , const areg::String & name /*= areg::String::getEmptyString()*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, areg::Event::EventType::EventRemoteServiceRequest, name)
{
}

RemoteRequestEvent::RemoteRequestEvent( const areg::InStream & stream )
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
AREG_IMPLEMENT_RUNTIME_EVENT(NotifyRequestEvent, areg::ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
NotifyRequestEvent::NotifyRequestEvent( const ProxyAddress & fromProxy
                                      , const areg::StubAddress & toStub
                                      , uint32_t msgId
                                      , areg::RequestType reqType
                                      , areg::Event::EventType eventType)
    : areg::ServiceRequestEvent ( fromProxy, toStub, msgId, reqType, eventType)
{
}

NotifyRequestEvent::NotifyRequestEvent(const areg::InStream & stream)
    : areg::ServiceRequestEvent ( stream )
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
                                                , const areg::StubAddress & toStub
                                                , uint32_t msgId
                                                , areg::RequestType reqType )
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, areg::Event::EventType::EventLocalNotifyRequest)
{
}

LocalNotifyRequestEvent::LocalNotifyRequestEvent( const areg::InStream & stream )
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
                                                  , const areg::StubAddress & toStub
                                                  , uint32_t msgId
                                                  , areg::RequestType reqType )
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, areg::Event::EventType::EventRemoteNotifyRequest)
{
}

RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const areg::InStream & stream )
    : NotifyRequestEvent ( stream )
{
}
