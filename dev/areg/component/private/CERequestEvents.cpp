/************************************************************************
 * \file        areg/component/private/CERequestEvents.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
#include "areg/component/CERequestEvents.hpp"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/private/CEServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// CERequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CERequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CERequestEvent, CEServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// CERequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CERequestEvent::CERequestEvent( const CEProxyAddress & fromSource
                              , const CEStubAddress & toTarget
                              , unsigned int reqId
                              , CEEvent::eEventType eventType )
    : CEServiceRequestEvent (fromSource, toTarget, reqId, NEService::REQUEST_CALL, eventType)
    , mData                 (reqId, (eventType & CEEvent::EventExternal) != 0 ? CEEventDataStream::EventDataExternal : CEEventDataStream::EventDataInternal)
{
    ; // do nothing
}

CERequestEvent::CERequestEvent( const CEEventDataStream & args
                              , const CEProxyAddress & fromSource
                              , const CEStubAddress& toTarget
                              , unsigned int reqId
                              , CEEvent::eEventType eventType
                              , const char* name /*= NULL*/ )
    : CEServiceRequestEvent (fromSource, toTarget, reqId, NEService::REQUEST_CALL, eventType)
    , mData                 (reqId, args, name)
{
    ; // do nothing
}

CERequestEvent::CERequestEvent( const IEInStream & stream )
    : CEServiceRequestEvent ( stream )
    , mData                 ( stream )
{
    ; // do nothing
}

CERequestEvent::~CERequestEvent( void )
{
    ; // do nothing
}

const IEInStream & CERequestEvent::ReadFromStream(const IEInStream & stream)
{
    CEServiceRequestEvent::ReadFromStream(stream);
    stream >> mData;
    return stream;
}

IEOutStream & CERequestEvent::WriteToStream(IEOutStream & stream) const
{
    CEServiceRequestEvent::WriteToStream(stream);
    stream << mData;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// CELocalRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CELocalRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CELocalRequestEvent, CERequestEvent)

//////////////////////////////////////////////////////////////////////////
// CERequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CELocalRequestEvent::CELocalRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CERequestEvent(fromSource, toTarget, reqId, CEEvent::EventLocalServiceRequest)
{
    ; // do nothing
}

CELocalRequestEvent::CELocalRequestEvent( const CEEventDataStream & args
                                        , const CEProxyAddress & fromSource
                                        , const CEStubAddress & toTarget
                                        , unsigned int reqId
                                        , const char* name /*= NULL*/ )
    : CERequestEvent(args, fromSource, toTarget, reqId, CEEvent::EventLocalServiceRequest, name)
{
    ; // do nothing
}

CELocalRequestEvent::CELocalRequestEvent( const IEInStream & stream )
    : CERequestEvent ( stream)
{
    ; // do nothing
}

CELocalRequestEvent::~CELocalRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CERemoteRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CERemoteRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CERemoteRequestEvent, CERequestEvent)

//////////////////////////////////////////////////////////////////////////
// CERequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CERemoteRequestEvent::CERemoteRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CERequestEvent(fromSource, toTarget, reqId, CEEvent::EventRemoteServiceRequest)
{
    ; // do nothing
}

CERemoteRequestEvent::CERemoteRequestEvent( const CEEventDataStream & args
                                          , const CEProxyAddress & fromSource
                                          , const CEStubAddress & toTarget
                                          , unsigned int reqId
                                          , const char* name /*= NULL*/ )
    : CERequestEvent(args, fromSource, toTarget, reqId, CEEvent::EventRemoteServiceRequest, name)
{
    ; // do nothing
}

CERemoteRequestEvent::CERemoteRequestEvent( const IEInStream & stream )
    : CERequestEvent ( stream)
{
    ASSERT(GetData().GetDataStream().IsExternalDataStream());
}

CERemoteRequestEvent::~CERemoteRequestEvent( void )
{
    ASSERT(GetData().GetDataStream().IsExternalDataStream());
}

//////////////////////////////////////////////////////////////////////////
// CENotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CENotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CENotifyRequestEvent, CEServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// CENotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CENotifyRequestEvent::CENotifyRequestEvent( const CEProxyAddress & fromProxy
                                          , const CEStubAddress & toStub
                                          , unsigned int msgId
                                          , NEService::eRequestType reqType
                                          , CEEvent::eEventType eventType)
    : CEServiceRequestEvent ( fromProxy, toStub, msgId, reqType, eventType)
{
    ; // do nothing
}

CENotifyRequestEvent::CENotifyRequestEvent(const IEInStream & stream)
    : CEServiceRequestEvent ( stream )
{
    ; // do nothing
}

CENotifyRequestEvent::~CENotifyRequestEvent(void)
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CELocalNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CELocalNotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CELocalNotifyRequestEvent, CENotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// CENotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CELocalNotifyRequestEvent::CELocalNotifyRequestEvent( const CEProxyAddress & fromProxy
                                                    , const CEStubAddress & toStub
                                                    , unsigned int msgId
                                                    , NEService::eRequestType reqType )
    : CENotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, CEEvent::EventLocalNotifyRequest)
{
    ; // do nothing
}

CELocalNotifyRequestEvent::CELocalNotifyRequestEvent( const IEInStream & stream )
    : CENotifyRequestEvent ( stream )
{
    ; // do nothing
}

CELocalNotifyRequestEvent::~CELocalNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CERemoteNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CERemoteNotifyRequestEvent class, runtime functions implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CERemoteNotifyRequestEvent, CENotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// CENotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CERemoteNotifyRequestEvent::CERemoteNotifyRequestEvent( const CEProxyAddress & fromProxy
                                                      , const CEStubAddress & toStub
                                                      , unsigned int msgId
                                                      , NEService::eRequestType reqType )
    : CENotifyRequestEvent  ( fromProxy, toStub, msgId, reqType, CEEvent::EventRemoteNotifyRequest)
{
    ; // do nothing
}

CERemoteNotifyRequestEvent::CERemoteNotifyRequestEvent( const IEInStream & stream )
    : CENotifyRequestEvent ( stream )
{
    ; // do nothing
}

CERemoteNotifyRequestEvent::~CERemoteNotifyRequestEvent( void )
{
    ; // do nothing
}
