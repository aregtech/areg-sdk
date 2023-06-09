/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceRequestEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Request Event implementation.
 *
 ************************************************************************/
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/StubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class, declare runtime event
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(ServiceRequestEvent, StubEvent)

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ServiceRequestEvent::ServiceRequestEvent( const ProxyAddress & proxyAddress
                                        , const StubAddress  & target
                                        , unsigned int reqId
                                        , NEService::eRequestType reqType
                                        , Event::eEventType eventType)
    : StubEvent     (target, eventType)
    , mProxySource  (proxyAddress)
    , mMessageId    (reqId)
    , mRequestType  (reqType)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
{
}

ServiceRequestEvent::ServiceRequestEvent(const IEInStream & stream)
    : StubEvent     (stream)
    , mProxySource  (stream)
    , mMessageId    (NEService::INVALID_MESSAGE_ID)
    , mRequestType  (NEService::eRequestType::Unprocessed)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
{
    stream >> mMessageId;
    stream >> mRequestType;
    stream >> mSequenceNr;
}

const IEInStream & ServiceRequestEvent::readStream(const IEInStream & stream)
{
    StubEvent::readStream(stream);
    stream >> mProxySource;
    stream >> mMessageId;
    stream >> mRequestType;
    stream >> mSequenceNr;
    return stream;
}

IEOutStream & ServiceRequestEvent::writeStream(IEOutStream & stream) const
{
    StubEvent::writeStream(stream);
    stream << mProxySource;
    stream << mMessageId;
    stream << mRequestType;
    stream << mSequenceNr;
    return stream;
}
