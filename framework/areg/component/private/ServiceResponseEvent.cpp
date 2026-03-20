/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceResponseEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Response Event implementation.
 *
 ************************************************************************/
#include "areg/component/ServiceResponseEvent.hpp"
namespace areg {


AREG_IMPLEMENT_RUNTIME_EVENT(ServiceResponseEvent, ProxyEvent)

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress & target
                                          , areg::ResultType result
                                          , uint32_t respId
                                          , areg::EventType eventType
                                          , const SequenceNumber & seqNr    /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ProxyEvent    (target, eventType)
    , mResponseId   (respId)
    , mResult       (result)
    , mSequenceNr   (seqNr)
{
}

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress& target, const ServiceResponseEvent& src )
    : ProxyEvent    (target, src.event_type())
    , mResponseId   (src.mResponseId)
    , mResult       (src.mResult)
    , mSequenceNr   (src.mSequenceNr)
{
}

ServiceResponseEvent::ServiceResponseEvent(const InStream & stream)
    : ProxyEvent    ( stream )
    , mResponseId   ( areg::INVALID_MESSAGE_ID )
    , mResult       ( areg::ResultType::Undefined )
    , mSequenceNr   ( areg::SEQUENCE_NUMBER_ANY )
{
    stream >> mResponseId;
    stream >> mResult;
    stream >> mSequenceNr;
}


ServiceResponseEvent* ServiceResponseEvent::clone_for_target( const ProxyAddress & target ) const
{
    return DEBUG_NEW ServiceResponseEvent(target, *this);
}

const InStream & ServiceResponseEvent::read_stream( const InStream & stream )
{
    ProxyEvent::read_stream(stream);
    stream >> mResponseId;
    stream >> mResult;
    stream >> mSequenceNr;
    return stream;
}

OutStream & ServiceResponseEvent::write_stream( OutStream & stream ) const
{
    ProxyEvent::write_stream(stream);
    stream << mResponseId;
    stream << mResult;
    stream << mSequenceNr;
    return stream;
}

} // namespace areg
