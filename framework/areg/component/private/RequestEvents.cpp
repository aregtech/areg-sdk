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
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RequestEvent::RequestEvent( const ProxyAddress & fromSource
                          , const StubAddress & toTarget
                          , uint32_t reqId
                          , areg::EventType eventType )
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, eventType)
{
}

RequestEvent::RequestEvent( const SharedBuffer & args
                          , const ProxyAddress & fromSource
                          , const StubAddress& toTarget
                          , uint32_t reqId
                          , areg::EventType eventType)
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, eventType)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

RequestEvent::RequestEvent( const EventEnvelope & envelope ) noexcept
    : ServiceRequestEvent( envelope )
{
}

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalRequestEvent::LocalRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId )
    : RequestEvent(fromSource, toTarget, reqId, areg::EventType::EventLocalRequest)
{
}

LocalRequestEvent::LocalRequestEvent( const SharedBuffer & args
                                    , const ProxyAddress & fromSource
                                    , const StubAddress & toTarget
                                    , uint32_t reqId)
    : RequestEvent(args, fromSource, toTarget, reqId, areg::EventType::EventLocalRequest)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteRequestEvent::RemoteRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId )
    : RequestEvent(fromSource, toTarget, reqId, areg::EventType::EventRemoteRequest)
{
}

RemoteRequestEvent::RemoteRequestEvent( const SharedBuffer & args
                                      , const ProxyAddress & fromSource
                                      , const StubAddress & toTarget
                                      , uint32_t reqId)
    : RequestEvent(args, fromSource, toTarget, reqId, areg::EventType::EventRemoteRequest)
{
}

RemoteRequestEvent::RemoteRequestEvent( const EventEnvelope & envelope ) noexcept
    : RequestEvent( envelope )
{
}

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
NotifyRequestEvent::NotifyRequestEvent( const ProxyAddress & fromProxy
                                      , const StubAddress & toStub
                                      , uint32_t msgId
                                      , areg::RequestType reqType
                                      , areg::EventType eventType)
    : ServiceRequestEvent ( fromProxy, toStub, msgId, reqType, eventType)
{
}

NotifyRequestEvent::NotifyRequestEvent( const EventEnvelope & envelope ) noexcept
    : ServiceRequestEvent( envelope )
{
}

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalNotifyRequestEvent::LocalNotifyRequestEvent( const ProxyAddress & fromProxy
                                                , const StubAddress & toStub
                                                , uint32_t msgId
                                                , areg::RequestType reqType)
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, areg::EventType::EventLocalNotifyRequest)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const ProxyAddress & fromProxy
                                                  , const StubAddress & toStub
                                                  , uint32_t msgId
                                                  , areg::RequestType reqType)
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, areg::EventType::EventRemoteNotifyRequest)
{
}

RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const EventEnvelope & envelope ) noexcept
    : NotifyRequestEvent( envelope )
{
}

} // namespace areg
