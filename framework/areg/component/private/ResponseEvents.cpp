/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ResponseEvents.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/private/ServiceManager.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(ResponseEvent, ServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ResponseEvent::ResponseEvent( const ProxyAddress & proxyTarget
                            , areg::ResultType result
                            , uint32_t respId
                            , areg::EventType eventType
                            , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/ )
    : ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
    , mData (respId, areg::is_external(eventType) ? EventDataStream::EventDataKind::External : EventDataStream::EventDataKind::Internal)
{
}

ResponseEvent::ResponseEvent( const EventDataStream & args
                            , const ProxyAddress & proxyTarget
                            , areg::ResultType result
                            , uint32_t respId
                            , areg::EventType eventType
                            , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
    , mData (respId, args)
{
}

ResponseEvent::ResponseEvent( const ProxyAddress& proxyTarget, const ResponseEvent& src )
    : ServiceResponseEvent(proxyTarget, static_cast<const ServiceResponseEvent &>(src))
    , mData (src.mData)
{
}

ResponseEvent::ResponseEvent(const InStream & stream)
    : ServiceResponseEvent(stream)
    , mData (stream)
{
}

const InStream & ResponseEvent::read_stream(const InStream & stream)
{
    ServiceResponseEvent::read_stream(stream);
    stream >> mData;
    return stream;
}

OutStream & ResponseEvent::write_stream(OutStream & stream) const
{
    ServiceResponseEvent::write_stream(stream);
    stream << mData;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(LocalResponseEvent, ResponseEvent)

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalResponseEvent::LocalResponseEvent( const ProxyAddress & proxyTarget
                                      , areg::ResultType result
                                      , uint32_t respId
                                      , const SequenceNumber & seqNr    /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(proxyTarget, result, respId, areg::EventType::EventLocalServiceResponse, seqNr)
{
}

LocalResponseEvent::LocalResponseEvent( const EventDataStream & args
                                      , const ProxyAddress & proxyTarget
                                      , areg::ResultType result
                                      , uint32_t respId
                                      , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(args, proxyTarget, result, respId, areg::EventType::EventLocalServiceResponse, seqNr)
{
}

LocalResponseEvent::LocalResponseEvent( const ProxyAddress& proxyTarget, const LocalResponseEvent & src )
    : ResponseEvent(proxyTarget, static_cast<const ResponseEvent &>(src))
{
}

LocalResponseEvent::LocalResponseEvent( const InStream & stream )
    : ResponseEvent(stream)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(RemoteResponseEvent, ResponseEvent)

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress & proxyTarget
                                        , areg::ResultType result
                                        , uint32_t respId
                                        , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(proxyTarget, result, respId, areg::EventType::EventRemoteServiceResponse, seqNr)
{
    ASSERT(data().data_stream().is_external_stream());
}

RemoteResponseEvent::RemoteResponseEvent( const EventDataStream & args
                                        , const ProxyAddress & proxyTarget
                                        , areg::ResultType result
                                        , uint32_t respId
                                        , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(args, proxyTarget, result, respId, areg::EventType::EventRemoteServiceResponse, seqNr)
{
    ASSERT(data().data_stream().is_external_stream());
}

RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress& proxyTarget, const RemoteResponseEvent & src )
    : ResponseEvent(proxyTarget, static_cast<const ResponseEvent &>(src))
{
    ASSERT(data().data_stream().is_external_stream());
}

RemoteResponseEvent::RemoteResponseEvent( const InStream & stream )
    : ResponseEvent(stream)
{
    ASSERT(data().data_stream().is_external_stream());
}

} // namespace areg
