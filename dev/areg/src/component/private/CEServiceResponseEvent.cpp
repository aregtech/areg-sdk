/************************************************************************
 * \file        areg/src/component/private/CEServiceResponseEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Response Event implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEServiceResponseEvent.hpp"


IMPLEMENT_RUNTIME_EVENT(CEServiceResponseEvent, CEProxyEvent)

CEServiceResponseEvent::CEServiceResponseEvent( const CEProxyAddress & target
                                              , NEService::eResultType result
                                              , unsigned int responseId
                                              , CEEvent::eEventType eventType
                                              , unsigned int seqNr    /*= NEService::SEQUENCE_NUMBER_NOTIFY*/)
    : CEProxyEvent  (target, eventType)
    , mResponseId   (responseId)
    , mResult       (result)
    , mSequenceNr   (seqNr)
{
    ; // do nothing
}

CEServiceResponseEvent::CEServiceResponseEvent( const CEProxyAddress& target, const CEServiceResponseEvent& src )
    : CEProxyEvent  (target, src.GetEventType())
    , mResponseId   (src.mResponseId)
    , mResult       (src.mResult)
    , mSequenceNr   (src.mSequenceNr)
{
    ; // do nothing
}

CEServiceResponseEvent::CEServiceResponseEvent(const IEInStream & stream)
    : CEProxyEvent  ( stream )
    , mResponseId   ( NEService::INVALID_MESSAGE_ID )
    , mResult       ( NEService::RESULT_UNDEFINED )
    , mSequenceNr   ( NEService::SEQUENCE_NUMBER_ANY )
{
    stream >> mResponseId;
    stream >> mResult;
    stream >> mSequenceNr;
}


CEServiceResponseEvent::~CEServiceResponseEvent( void )
{
    ; // do nothing
}

CEServiceResponseEvent* CEServiceResponseEvent::CloneEventForTarget( const CEProxyAddress & target ) const
{
    return DEBUG_NEW CEServiceResponseEvent(target, *this);
}

const IEInStream & CEServiceResponseEvent::ReadFromStream( const IEInStream & stream )
{
    CEProxyEvent::ReadFromStream(stream);
    stream >> mResponseId;
    stream >> mResult;
    stream >> mSequenceNr;
    return stream;
}

IEOutStream & CEServiceResponseEvent::WriteToStream( IEOutStream & stream ) const
{
    CEProxyEvent::WriteToStream(stream);
    stream << mResponseId;
    stream << mResult;
    stream << mSequenceNr;
    return stream;
}
