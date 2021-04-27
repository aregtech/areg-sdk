/************************************************************************
 * \file        areg/component/private/CEResponseEvents.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
#include "areg/component/CEResponseEvents.hpp"
#include "areg/component/private/CEServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// CEResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CEResponseEvent, CEServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// CEResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEResponseEvent::CEResponseEvent( const CEProxyAddress & proxyTarget
                                , NEService::eResultType result
                                , unsigned int respId
                                , CEEvent::eEventType eventType
                                , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/ )
    : CEServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
    , mData                 (respId, (eventType & CEEvent::EventExternal) != 0 ? CEEventDataStream::EventDataExternal : CEEventDataStream::EventDataInternal)
{
    ; // do nothing
}

CEResponseEvent::CEResponseEvent( const CEEventDataStream & args
                                , const CEProxyAddress & proxyTarget
                                , NEService::eResultType result
                                , unsigned int respId
                                , CEEvent::eEventType eventType
                                , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                                , const char* name    /*= NULL*/ )
    : CEServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
    , mData                 (respId, args, name)
{
    ; // do nothing
}

CEResponseEvent::CEResponseEvent( const CEProxyAddress& proxyTarget, const CEResponseEvent& src )
    : CEServiceResponseEvent(proxyTarget, static_cast<const CEServiceResponseEvent &>(src))
    , mData                 (src.mData)
{
    ; // do nothing
}

CEResponseEvent::CEResponseEvent(const IEInStream & stream)
    : CEServiceResponseEvent(stream)
    , mData                 (stream)
{
    ; // do nothing
}

CEResponseEvent::~CEResponseEvent( void )
{
    ; // do nothing
}

const IEInStream & CEResponseEvent::ReadFromStream(const IEInStream & stream)
{
    CEServiceResponseEvent::ReadFromStream(stream);
    stream >> mData;
    return stream;
}

IEOutStream & CEResponseEvent::WriteToStream(IEOutStream & stream) const
{
    CEServiceResponseEvent::WriteToStream(stream);
    stream << mData;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// CELocalResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CELocalResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CELocalResponseEvent, CEResponseEvent)

//////////////////////////////////////////////////////////////////////////
// CELocalResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CELocalResponseEvent::CELocalResponseEvent( const CEProxyAddress & proxyTarget
                                          , NEService::eResultType result
                                          , unsigned int respId
                                          , unsigned int seqNr    /*= NEService::SEQUENCE_NUMBER_NOTIFY*/)
    : CEResponseEvent(proxyTarget, result, respId, CEEvent::EventLocalServiceResponse, seqNr)
{
    ; // do nothing
}

CELocalResponseEvent::CELocalResponseEvent( const CEEventDataStream & args
                                          , const CEProxyAddress & proxyTarget
                                          , NEService::eResultType result
                                          , unsigned int respId
                                          , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                                          , const char* name    /*= NULL*/ )
    : CEResponseEvent(args, proxyTarget, result, respId, CEEvent::EventLocalServiceResponse, seqNr, name)
{
    ; // do nothing
}

CELocalResponseEvent::CELocalResponseEvent( const CEProxyAddress& proxyTarget, const CELocalResponseEvent & src )
    : CEResponseEvent(proxyTarget, static_cast<const CEResponseEvent &>(src))
{
    ; // do nothing
}

CELocalResponseEvent::CELocalResponseEvent( const IEInStream & stream )
    : CEResponseEvent(stream)
{
    ; // do nothing
}

CELocalResponseEvent::~CELocalResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CERemoteResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CERemoteResponseEvent class runtime function implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CERemoteResponseEvent, CEResponseEvent)

//////////////////////////////////////////////////////////////////////////
// CERemoteResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CERemoteResponseEvent::CERemoteResponseEvent( const CEProxyAddress & proxyTarget
                                            , NEService::eResultType result
                                            , unsigned int respId
                                            , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/)
    : CEResponseEvent(proxyTarget, result, respId, CEEvent::EventRemoteServiceResponse, seqNr)
{
    ASSERT(GetData().GetDataStream().IsExternalDataStream());
}

CERemoteResponseEvent::CERemoteResponseEvent( const CEEventDataStream & args
                                            , const CEProxyAddress & proxyTarget
                                            , NEService::eResultType result
                                            , unsigned int respId
                                            , unsigned int seqNr  /*= NEService::SEQUENCE_NUMBER_NOTIFY*/
                                            , const char* name    /*= NULL*/ )
    : CEResponseEvent(args, proxyTarget, result, respId, CEEvent::EventRemoteServiceResponse, seqNr, name)
{
    ASSERT(GetData().GetDataStream().IsExternalDataStream());
}

CERemoteResponseEvent::CERemoteResponseEvent( const CEProxyAddress& proxyTarget, const CERemoteResponseEvent & src )
    : CEResponseEvent(proxyTarget, static_cast<const CEResponseEvent &>(src))
{
    ASSERT(GetData().GetDataStream().IsExternalDataStream());
}

CERemoteResponseEvent::CERemoteResponseEvent( const IEInStream & stream )
    : CEResponseEvent(stream)
{
    ASSERT(GetData().GetDataStream().IsExternalDataStream());
}

CERemoteResponseEvent::~CERemoteResponseEvent( void )
{
    ; // do nothing
}
