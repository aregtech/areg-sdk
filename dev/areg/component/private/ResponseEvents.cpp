/************************************************************************
 * \file        areg/component/private/ResponseEvents.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/private/ServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(ResponseEvent, ServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ResponseEvent::ResponseEvent( const ProxyAddress & proxyTarget
                            , NEService::eResultType result
                            , unsigned int respId
                            , Event::eEventType eventType
                            , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/ )
    : ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
    , mData                 (respId, (eventType & Event::EventExternal) != 0 ? EventDataStream::EventDataExternal : EventDataStream::EventDataInternal)
{
    ; // do nothing
}

ResponseEvent::ResponseEvent( const EventDataStream & args
                            , const ProxyAddress & proxyTarget
                            , NEService::eResultType result
                            , unsigned int respId
                            , Event::eEventType eventType
                            , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                            , const char* name    /*= NULL*/ )
    : ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
    , mData                 (respId, args, name)
{
    ; // do nothing
}

ResponseEvent::ResponseEvent( const ProxyAddress& proxyTarget, const ResponseEvent& src )
    : ServiceResponseEvent(proxyTarget, static_cast<const ServiceResponseEvent &>(src))
    , mData                 (src.mData)
{
    ; // do nothing
}

ResponseEvent::ResponseEvent(const IEInStream & stream)
    : ServiceResponseEvent(stream)
    , mData                 (stream)
{
    ; // do nothing
}

ResponseEvent::~ResponseEvent( void )
{
    ; // do nothing
}

const IEInStream & ResponseEvent::readStream(const IEInStream & stream)
{
    ServiceResponseEvent::readStream(stream);
    stream >> mData;
    return stream;
}

IEOutStream & ResponseEvent::writeStream(IEOutStream & stream) const
{
    ServiceResponseEvent::writeStream(stream);
    stream << mData;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(LocalResponseEvent, ResponseEvent)

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalResponseEvent::LocalResponseEvent( const ProxyAddress & proxyTarget
                                          , NEService::eResultType result
                                          , unsigned int respId
                                          , unsigned int seqNr    /*= NEService::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(proxyTarget, result, respId, Event::EventLocalServiceResponse, seqNr)
{
    ; // do nothing
}

LocalResponseEvent::LocalResponseEvent( const EventDataStream & args
                                          , const ProxyAddress & proxyTarget
                                          , NEService::eResultType result
                                          , unsigned int respId
                                          , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                                          , const char* name    /*= NULL*/ )
    : ResponseEvent(args, proxyTarget, result, respId, Event::EventLocalServiceResponse, seqNr, name)
{
    ; // do nothing
}

LocalResponseEvent::LocalResponseEvent( const ProxyAddress& proxyTarget, const LocalResponseEvent & src )
    : ResponseEvent(proxyTarget, static_cast<const ResponseEvent &>(src))
{
    ; // do nothing
}

LocalResponseEvent::LocalResponseEvent( const IEInStream & stream )
    : ResponseEvent(stream)
{
    ; // do nothing
}

LocalResponseEvent::~LocalResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(RemoteResponseEvent, ResponseEvent)

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress & proxyTarget
                                            , NEService::eResultType result
                                            , unsigned int respId
                                            , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(proxyTarget, result, respId, Event::EventRemoteServiceResponse, seqNr)
{
    ASSERT(getData().getDataStream().isExternalDataStream());
}

RemoteResponseEvent::RemoteResponseEvent( const EventDataStream & args
                                            , const ProxyAddress & proxyTarget
                                            , NEService::eResultType result
                                            , unsigned int respId
                                            , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                                            , const char* name    /*= NULL*/ )
    : ResponseEvent(args, proxyTarget, result, respId, Event::EventRemoteServiceResponse, seqNr, name)
{
    ASSERT(getData().getDataStream().isExternalDataStream());
}

RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress& proxyTarget, const RemoteResponseEvent & src )
    : ResponseEvent(proxyTarget, static_cast<const ResponseEvent &>(src))
{
    ASSERT(getData().getDataStream().isExternalDataStream());
}

RemoteResponseEvent::RemoteResponseEvent( const IEInStream & stream )
    : ResponseEvent(stream)
{
    ASSERT(getData().getDataStream().isExternalDataStream());
}

RemoteResponseEvent::~RemoteResponseEvent( void )
{
    ; // do nothing
}
