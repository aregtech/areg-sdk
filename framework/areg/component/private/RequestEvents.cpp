/************************************************************************
 * \file        areg/component/private/RequestEvents.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    : ServiceRequestEvent(fromSource, toTarget, reqId, NEService::REQUEST_CALL, eventType)
    , mData(reqId, (eventType & Event::EventExternal) != 0 ? EventDataStream::EventDataExternal : EventDataStream::EventDataInternal)
{
    ; // do nothing
}

RequestEvent::RequestEvent( const EventDataStream & args
                          , const ProxyAddress & fromSource
                          , const StubAddress& toTarget
                          , unsigned int reqId
                          , Event::eEventType eventType
                          , const char* name /*= NULL*/ )
    : ServiceRequestEvent(fromSource, toTarget, reqId, NEService::REQUEST_CALL, eventType)
    , mData(reqId, args, name)
{
    ; // do nothing
}

RequestEvent::RequestEvent( const IEInStream & stream )
    : ServiceRequestEvent( stream )
    , mData( stream )
{
    ; // do nothing
}

RequestEvent::~RequestEvent( void )
{
    ; // do nothing
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
    : RequestEvent(fromSource, toTarget, reqId, Event::EventLocalServiceRequest)
{
    ; // do nothing
}

LocalRequestEvent::LocalRequestEvent( const EventDataStream & args
                                        , const ProxyAddress & fromSource
                                        , const StubAddress & toTarget
                                        , unsigned int reqId
                                        , const char* name /*= NULL*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, Event::EventLocalServiceRequest, name)
{
    ; // do nothing
}

LocalRequestEvent::LocalRequestEvent( const IEInStream & stream )
    : RequestEvent ( stream)
{
    ; // do nothing
}

LocalRequestEvent::~LocalRequestEvent( void )
{
    ; // do nothing
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
    : RequestEvent(fromSource, toTarget, reqId, Event::EventRemoteServiceRequest)
{
    ; // do nothing
}

RemoteRequestEvent::RemoteRequestEvent( const EventDataStream & args
                                          , const ProxyAddress & fromSource
                                          , const StubAddress & toTarget
                                          , unsigned int reqId
                                          , const char* name /*= NULL*/ )
    : RequestEvent(args, fromSource, toTarget, reqId, Event::EventRemoteServiceRequest, name)
{
    ; // do nothing
}

RemoteRequestEvent::RemoteRequestEvent( const IEInStream & stream )
    : RequestEvent ( stream)
{
    ASSERT(getData().getDataStream().isExternalDataStream());
}

RemoteRequestEvent::~RemoteRequestEvent( void )
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
    ; // do nothing
}

NotifyRequestEvent::NotifyRequestEvent(const IEInStream & stream)
    : ServiceRequestEvent ( stream )
{
    ; // do nothing
}

NotifyRequestEvent::~NotifyRequestEvent(void)
{
    ; // do nothing
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
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, Event::EventLocalNotifyRequest)
{
    ; // do nothing
}

LocalNotifyRequestEvent::LocalNotifyRequestEvent( const IEInStream & stream )
    : NotifyRequestEvent ( stream )
{
    ; // do nothing
}

LocalNotifyRequestEvent::~LocalNotifyRequestEvent( void )
{
    ; // do nothing
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
    : NotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, Event::EventRemoteNotifyRequest)
{
    ; // do nothing
}

RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const IEInStream & stream )
    : NotifyRequestEvent ( stream )
{
    ; // do nothing
}

RemoteNotifyRequestEvent::~RemoteNotifyRequestEvent( void )
{
    ; // do nothing
}
