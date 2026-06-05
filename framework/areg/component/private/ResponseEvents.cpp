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
// ResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ResponseEvent::ResponseEvent( const ProxyAddress & proxyTarget
                            , areg::ResultType result
                            , uint32_t respId
                            , areg::EventType eventType
                            , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                            , uint32_t initSize             /*= 0u*/ )
    : ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr, initSize)
{
}

ResponseEvent::ResponseEvent( const SharedBuffer & args
                            , const ProxyAddress & proxyTarget
                            , areg::ResultType result
                            , uint32_t respId
                            , areg::EventType eventType
                            , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

ResponseEvent::ResponseEvent( const ProxyAddress& proxyTarget, const ResponseEvent& src )
    : ServiceResponseEvent(proxyTarget, static_cast<const ServiceResponseEvent &>(src))
{
}

ResponseEvent::ResponseEvent( const EventEnvelope & envelope ) noexcept
    : ServiceResponseEvent( envelope )
{
}

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
LocalResponseEvent::LocalResponseEvent( const ProxyAddress & proxyTarget
                                      , areg::ResultType result
                                      , uint32_t respId
                                      , const SequenceNumber & seqNr    /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(proxyTarget, result, respId, areg::EventType::EventLocalResponse, seqNr)
{
}

LocalResponseEvent::LocalResponseEvent( const SharedBuffer & args
                                      , const ProxyAddress & proxyTarget
                                      , areg::ResultType result
                                      , uint32_t respId
                                      , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(args, proxyTarget, result, respId, areg::EventType::EventLocalResponse, seqNr)
{
}

LocalResponseEvent::LocalResponseEvent( const ProxyAddress& proxyTarget, const LocalResponseEvent & src )
    : ResponseEvent(proxyTarget, static_cast<const ResponseEvent &>(src))
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress & proxyTarget
                                        , areg::ResultType result
                                        , uint32_t respId
                                        , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                                        , uint32_t initSize             /*= 0u*/ )
    : ResponseEvent(proxyTarget, result, respId, areg::EventType::EventRemoteResponse, seqNr, initSize)
{
}

RemoteResponseEvent::RemoteResponseEvent( const SharedBuffer & args
                                        , const ProxyAddress & proxyTarget
                                        , areg::ResultType result
                                        , uint32_t respId
                                        , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ResponseEvent(args, proxyTarget, result, respId, areg::EventType::EventRemoteResponse, seqNr)
{
}

RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress& proxyTarget, const RemoteResponseEvent & src )
    : ResponseEvent(proxyTarget, static_cast<const ResponseEvent &>(src))
{
}

RemoteResponseEvent::RemoteResponseEvent( const EventEnvelope & envelope ) noexcept
    : ResponseEvent( envelope )
{
}

} // namespace areg
