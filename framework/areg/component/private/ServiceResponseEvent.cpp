/************************************************************************
 * \file        areg/component/private/ServiceResponseEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    ; // do nothing
}

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress& target, const ServiceResponseEvent& src )
    : ProxyEvent    (target, src.getEventType())
    , mResponseId   (src.mResponseId)
    , mResult       (src.mResult)
    , mSequenceNr   (src.mSequenceNr)
{
    ; // do nothing
}

ServiceResponseEvent::ServiceResponseEvent(const IEInStream & stream)
    : ProxyEvent    ( stream )
    , mResponseId   ( NEService::INVALID_MESSAGE_ID )
    , mResult       ( NEService::RESULT_UNDEFINED )
    , mSequenceNr   ( NEService::SEQUENCE_NUMBER_ANY )
{
    stream >> mResponseId;
    stream >> mResult;
    stream >> mSequenceNr;
}


ServiceResponseEvent::~ServiceResponseEvent( void )
{
    ; // do nothing
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
