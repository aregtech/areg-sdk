/************************************************************************
 * \file        areg/component/private/ServiceRequestEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    : StubEvent   (target, eventType)
    , mProxySource  (proxyAddress)
    , mMessageId    (reqId)
    , mRequestType  (reqType)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
{
    ; // do nothing
}

ServiceRequestEvent::ServiceRequestEvent(const IEInStream & stream)
    : StubEvent   (stream)
    , mProxySource  (stream)
    , mMessageId    (NEService::INVALID_MESSAGE_ID)
    , mRequestType  (NEService::REQUEST_UNPROCESSED)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
{
    stream >> mMessageId;
    stream >> mRequestType;
    stream >> mSequenceNr;
}

ServiceRequestEvent::~ServiceRequestEvent( void )
{
    ; // do nothing
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
