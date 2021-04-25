/************************************************************************
 * \file        areg/src/component/private/CEServiceRequestEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Request Event implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEServiceRequestEvent.hpp"
#include "areg/src/component/CEStubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// CEServiceRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEServiceRequestEvent class, declare runtime event
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CEServiceRequestEvent, CEStubEvent)

//////////////////////////////////////////////////////////////////////////
// CEServiceRequestEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEServiceRequestEvent::CEServiceRequestEvent( const CEProxyAddress & proxyAddress
                                            , const CEStubAddress  & target
                                            , unsigned int reqId
                                            , NEService::eRequestType reqType
                                            , CEEvent::eEventType eventType)
    : CEStubEvent   (target, eventType)
    , mProxySource  (proxyAddress)
    , mMessageId    (reqId)
    , mRequestType  (reqType)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
{
    ; // do nothing
}

CEServiceRequestEvent::CEServiceRequestEvent(const IEInStream & stream)
    : CEStubEvent   (stream)
    , mProxySource  (stream)
    , mMessageId    (NEService::INVALID_MESSAGE_ID)
    , mRequestType  (NEService::REQUEST_UNPROCESSED)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
{
    stream >> mMessageId;
    stream >> mRequestType;
    stream >> mSequenceNr;
}

CEServiceRequestEvent::~CEServiceRequestEvent( void )
{
    ; // do nothing
}

const IEInStream & CEServiceRequestEvent::ReadFromStream(const IEInStream & stream)
{
    CEStubEvent::ReadFromStream(stream);
    stream >> mProxySource;
    stream >> mMessageId;
    stream >> mRequestType;
    stream >> mSequenceNr;
    return stream;
}

IEOutStream & CEServiceRequestEvent::WriteToStream(IEOutStream & stream) const
{
    CEStubEvent::WriteToStream(stream);
    stream << mProxySource;
    stream << mMessageId;
    stream << mRequestType;
    stream << mSequenceNr;
    return stream;
}
