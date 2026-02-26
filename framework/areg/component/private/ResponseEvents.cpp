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

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // ResponseEvent class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // ResponseEvent class runtime function implementation
    //////////////////////////////////////////////////////////////////////////
    AREG_IMPLEMENT_RUNTIME_EVENT(areg::ResponseEvent, areg::ServiceResponseEvent)

    //////////////////////////////////////////////////////////////////////////
    // ResponseEvent class Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    areg::ResponseEvent::ResponseEvent( const areg::ProxyAddress & proxyTarget
                                , areg::ResultType result
                                , uint32_t respId
                                , areg::Event::EventType eventType
                                , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/ )
        : areg::ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
        , mData (respId, areg::Event::isExternal(eventType) ? areg::EventDataStream::EventDataKind::External : areg::EventDataStream::EventDataKind::Internal)
    {
    }

    areg::ResponseEvent::ResponseEvent( const areg::EventDataStream & args
                                , const areg::ProxyAddress & proxyTarget
                                , areg::ResultType result
                                , uint32_t respId
                                , areg::Event::EventType eventType
                                , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                                , const areg::String & name /*= areg::String::getEmptyString()*/ )
        : areg::ServiceResponseEvent(proxyTarget, result, respId, eventType, seqNr)
        , mData (respId, args, name)
    {
    }

    areg::ResponseEvent::ResponseEvent( const areg::ProxyAddress& proxyTarget, const areg::ResponseEvent& src )
        : areg::ServiceResponseEvent(proxyTarget, static_cast<const areg::ServiceResponseEvent &>(src))
        , mData (src.mData)
    {
    }

    areg::ResponseEvent::ResponseEvent(const areg::InStream & stream)
        : areg::ServiceResponseEvent(stream)
        , mData (stream)
    {
    }

    const areg::InStream & areg::ResponseEvent::readStream(const areg::InStream & stream)
    {
        areg::ServiceResponseEvent::readStream(stream);
        stream >> mData;
        return stream;
    }

    areg::OutStream & areg::ResponseEvent::writeStream(areg::OutStream & stream) const
    {
        areg::ServiceResponseEvent::writeStream(stream);
        stream << mData;
        return stream;
    }

    //////////////////////////////////////////////////////////////////////////
    // LocalResponseEvent class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // LocalResponseEvent class runtime function implementation
    //////////////////////////////////////////////////////////////////////////
    AREG_IMPLEMENT_RUNTIME_EVENT(areg::LocalResponseEvent, areg::ResponseEvent)

    //////////////////////////////////////////////////////////////////////////
    // LocalResponseEvent class Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    areg::LocalResponseEvent::LocalResponseEvent( const areg::ProxyAddress & proxyTarget
                                        , areg::ResultType result
                                        , uint32_t respId
                                        , const SequenceNumber & seqNr    /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
        : areg::ResponseEvent(proxyTarget, result, respId, areg::Event::EventType::EventLocalServiceResponse, seqNr)
    {
    }

    areg::LocalResponseEvent::LocalResponseEvent( const areg::EventDataStream & args
                                        , const areg::ProxyAddress & proxyTarget
                                        , areg::ResultType result
                                        , uint32_t respId
                                        , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                                        , const areg::String & name /*= areg::String::getEmptyString()*/ )
        : areg::ResponseEvent(args, proxyTarget, result, respId, areg::Event::EventType::EventLocalServiceResponse, seqNr, name)
    {
    }

    areg::LocalResponseEvent::LocalResponseEvent( const areg::ProxyAddress& proxyTarget, const areg::LocalResponseEvent & src )
        : areg::ResponseEvent(proxyTarget, static_cast<const areg::ResponseEvent &>(src))
    {
    }

    areg::LocalResponseEvent::LocalResponseEvent( const areg::InStream & stream )
        : areg::ResponseEvent(stream)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // RemoteResponseEvent class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // RemoteResponseEvent class runtime function implementation
    //////////////////////////////////////////////////////////////////////////
    AREG_IMPLEMENT_RUNTIME_EVENT(areg::RemoteResponseEvent, areg::ResponseEvent)

    //////////////////////////////////////////////////////////////////////////
    // RemoteResponseEvent class Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    areg::RemoteResponseEvent::RemoteResponseEvent( const areg::ProxyAddress & proxyTarget
                                            , areg::ResultType result
                                            , uint32_t respId
                                            , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
        : areg::ResponseEvent(proxyTarget, result, respId, areg::Event::EventType::EventRemoteServiceResponse, seqNr)
    {
        ASSERT(getData().getDataStream().isExternalDataStream());
    }

    areg::RemoteResponseEvent::RemoteResponseEvent( const areg::EventDataStream & args
                                            , const areg::ProxyAddress & proxyTarget
                                            , areg::ResultType result
                                            , uint32_t respId
                                            , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                                            , const areg::String & name /*= areg::String::getEmptyString()*/ )
        : areg::ResponseEvent(args, proxyTarget, result, respId, areg::Event::EventType::EventRemoteServiceResponse, seqNr, name)
    {
        ASSERT(getData().getDataStream().isExternalDataStream());
    }

    areg::RemoteResponseEvent::RemoteResponseEvent( const areg::ProxyAddress& proxyTarget, const areg::RemoteResponseEvent & src )
        : areg::ResponseEvent(proxyTarget, static_cast<const areg::ResponseEvent &>(src))
    {
        ASSERT(getData().getDataStream().isExternalDataStream());
    }

    areg::RemoteResponseEvent::RemoteResponseEvent( const areg::InStream & stream )
        : areg::ResponseEvent(stream)
    {
        ASSERT(getData().getDataStream().isExternalDataStream());
    }

} // namespace areg