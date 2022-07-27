/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ResponseEvents.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
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
    , mData (respId, Event::isExternal(eventType) ? EventDataStream::eEventData::EventDataExternal : EventDataStream::eEventData::EventDataInternal)
{
}

ResponseEvent::ResponseEvent( const EventDataStream & args
                            , const ProxyAddress & proxyTarget
                            , NEService::eResultType result
                            , unsigned int respId
                            , Event::eEventType eventType
                            , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                            , const String & name /*= String::EmptyString*/ )
    : ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
    , mData (respId, args, name)
{
}

ResponseEvent::ResponseEvent( const ProxyAddress& proxyTarget, const ResponseEvent& src )
    : ServiceResponseEvent(proxyTarget, static_cast<const ServiceResponseEvent &>(src))
    , mData (src.mData)
{
}

ResponseEvent::ResponseEvent(const IEInStream & stream)
    : ServiceResponseEvent(stream)
    , mData (stream)
{
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
    : ResponseEvent(proxyTarget, result, respId, Event::eEventType::EventLocalServiceResponse, seqNr)
{
}

LocalResponseEvent::LocalResponseEvent( const EventDataStream & args
                                      , const ProxyAddress & proxyTarget
                                      , NEService::eResultType result
                                      , unsigned int respId
                                      , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                                      , const String & name /*= String::EmptyString*/ )
    : ResponseEvent(args, proxyTarget, result, respId, Event::eEventType::EventLocalServiceResponse, seqNr, name)
{
}

LocalResponseEvent::LocalResponseEvent( const ProxyAddress& proxyTarget, const LocalResponseEvent & src )
    : ResponseEvent(proxyTarget, static_cast<const ResponseEvent &>(src))
{
}

LocalResponseEvent::LocalResponseEvent( const IEInStream & stream )
    : ResponseEvent(stream)
{
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
    : ResponseEvent(proxyTarget, result, respId, Event::eEventType::EventRemoteServiceResponse, seqNr)
{
    ASSERT(getData().getDataStream().isExternalDataStream());
}

RemoteResponseEvent::RemoteResponseEvent( const EventDataStream & args
                                        , const ProxyAddress & proxyTarget
                                        , NEService::eResultType result
                                        , unsigned int respId
                                        , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                                        , const String & name /*= String::EmptyString*/ )
    : ResponseEvent(args, proxyTarget, result, respId, Event::eEventType::EventRemoteServiceResponse, seqNr, name)
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
