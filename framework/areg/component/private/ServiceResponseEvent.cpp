/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceResponseEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Response Event implementation.
 *
 ************************************************************************/
#include "areg/component/ServiceResponseEvent.hpp"


IMPLEMENT_RUNTIME_EVENT(ServiceResponseEvent, ProxyEvent)

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress & target
                                          , NEService::eResultType result
                                          , unsigned int responseId
                                          , Event::eEventType eventType
                                          , unsigned int seqNr    /*= NEService::SEQUENCE_NUMBER_NOTIFY*/)
    : ProxyEvent    (target, eventType)
    , mResponseId   (responseId)
    , mResult       (result)
    , mSequenceNr   (seqNr)
{
}

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress& target, const ServiceResponseEvent& src )
    : ProxyEvent    (target, src.getEventType())
    , mResponseId   (src.mResponseId)
    , mResult       (src.mResult)
    , mSequenceNr   (src.mSequenceNr)
{
}

ServiceResponseEvent::ServiceResponseEvent(const IEInStream & stream)
    : ProxyEvent    ( stream )
    , mResponseId   ( NEService::INVALID_MESSAGE_ID )
    , mResult       ( NEService::eResultType::Undefined )
    , mSequenceNr   ( NEService::SEQUENCE_NUMBER_ANY )
{
    stream >> mResponseId;
    stream >> mResult;
    stream >> mSequenceNr;
}


ServiceResponseEvent* ServiceResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return DEBUG_NEW ServiceResponseEvent(target, *this);
}

const IEInStream & ServiceResponseEvent::readStream( const IEInStream & stream )
{
    ProxyEvent::readStream(stream);
    stream >> mResponseId;
    stream >> mResult;
    stream >> mSequenceNr;
    return stream;
}

IEOutStream & ServiceResponseEvent::writeStream( IEOutStream & stream ) const
{
    ProxyEvent::writeStream(stream);
    stream << mResponseId;
    stream << mResult;
    stream << mSequenceNr;
    return stream;
}
