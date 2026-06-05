/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Base implementation.
 *
 ************************************************************************/
#include "areg/component/StubBase.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/private/StubConnectEvent.hpp"

#include "areg/logging/areg_log.h"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// StubBase class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_StubBase, startup_service_interface);
DEF_LOG_SCOPE(areg_component_StubBase, shutdown_service_intrface);
DEF_LOG_SCOPE(areg_component_StubBase, error_all_requests);
DEF_LOG_SCOPE(areg_component_StubBase, send_update_event);
DEF_LOG_SCOPE(areg_component_StubBase, send_busy_response);
DEF_LOG_SCOPE(areg_component_StubBase, consumer_connected);
DEF_LOG_SCOPE(areg_component_StubBase, add_notification_listener);

//////////////////////////////////////////////////////////////////////////
// StubBase::Listener implementation
//////////////////////////////////////////////////////////////////////////

bool StubBase::Listener::operator == ( const StubBase::Listener & other ) const noexcept
{
    if (this == &other)
        return true;
    
    ASSERT(mMessageId == other.mMessageId);
    if (areg::SEQUENCE_NUMBER_ANY == other.mSequenceNr)
    {
        return true;
    }
    else if (mSequenceNr == other.mSequenceNr || mSequenceNr == static_cast<uint32_t>(-1 * static_cast<int32_t>(mSequenceNr == other.mSequenceNr)))
    {
        return true;
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////
// StubBase implementation
//////////////////////////////////////////////////////////////////////////

StubBase::MapProviderResource& StubBase::map_providers() noexcept
{
    static StubBase::MapProviderResource   _mapProviders;
    return _mapProviders;
}

StubBase::StubBase( Component & masterComp, const areg::InterfaceData & siData )
    : StubEventConsumer   ( mAddress )

    , mComponent            (masterComp)
    , mInterface            (siData)
    , mAddress              (siData, masterComp.address().role_name(), masterComp.address().thread_address().name())
    , mConnectionStatus     ( areg::ServiceConnectionState::Disconnected )
    , mListenerMap          ( )
    , mCurrMsgId            ( INVALID_MESSAGE_ID )
    , mCurrIndex            ( 0 )
    , mMapSessions          ( )
    , mSessionId            (0)
{
    map_providers().register_resource_object(static_cast<uint32_t>(mAddress), this);
    masterComp.register_service_provider(self());
}

StubBase::~StubBase()
{
    map_providers().unregister_resource_object(static_cast<uint32_t>(mAddress));
}

bool StubBase::is_busy( uint32_t reqId ) const noexcept
{
    StubListenerMap::MAPPOS pos = mListenerMap.find(reqId);
    if (!mListenerMap.is_valid_position(pos))
        return false;

    const StubListenerList& subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if (subVec.value_at(i).mSequenceNr != 0)
            return true;
    }

    return false;
}

SessionID StubBase::unblock_current_request()
{
    SessionID result = StubBase::INVALID_SESSION_ID;
    if (mCurrMsgId != INVALID_MESSAGE_ID)
    {
        StubListenerMap::MAPPOS pos = mListenerMap.find(mCurrMsgId);
        if (mListenerMap.is_valid_position(pos))
        {
            StubListenerList& subVec = pos->second;
            if (mCurrIndex < subVec.size())
            {
                Listener listener = subVec.value_at(mCurrIndex);
                // Swap-and-pop: move last element into this slot, then shrink
                uint32_t last = subVec.size() - 1;
                if (mCurrIndex != last)
                    subVec.set_value_at(mCurrIndex, subVec.value_at(last));
    
                subVec.remove_at(last);
                result = ++mSessionId;
                mMapSessions.set_value_at(result, listener);
            }
        }
    
        mCurrMsgId = INVALID_MESSAGE_ID;
    }

    return result;
}

bool StubBase::prepare_response( SessionID sessionId )
{
    bool result{ false };
    StubBase::Listener listener;
    if (mMapSessions.remove_at(sessionId, listener))
    {
        mListenerMap[listener.mMessageId].add(listener);
        result = true;
    }
    return result;
}

void StubBase::prepare_request( Listener & listener, const SequenceNumber & seqNr, uint32_t respId )
{
    listener.mMessageId = respId;
    // If a notification subscriber already exists for this response ID, negate seqNr to encode
    // "unblocked" state so the broadcast path can distinguish it from a fresh one-time request.
    StubListenerList& subVec = mListenerMap[respId];
    bool hasNotifyEntry = false;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if (subVec.value_at(i).mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY)
        {
            hasNotifyEntry = true;
            break;
        }
    }

    listener.mSequenceNr = hasNotifyEntry ? static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(seqNr)) : seqNr;
    subVec.add(listener);
    mCurrMsgId = respId;
    mCurrIndex  = subVec.size() - 1;
}

uint32_t StubBase::find_listeners( uint32_t reqId, StubListenerList & out_listners ) const
{
    StubListenerMap::MAPPOS pos = mListenerMap.find(reqId);
    if (mListenerMap.is_valid_position(pos))
        out_listners.append(pos->second);
    return out_listners.size();
}

void StubBase::clear_all_listeners( const ProxyAddress & whichProxy, IntegerArray & removedIDs )
{
    for (StubListenerMap::MAPPOS mapPos = mListenerMap.first_position(); mListenerMap.is_valid_position(mapPos); mapPos = mListenerMap.next_position(mapPos))
    {
        StubListenerList& subVec = mapPos->second;
        uint32_t i = 0;
        while (i < subVec.size())
        {
            if (subVec.value_at(i).mProxy != whichProxy)
            {
                ++i;
                continue;
            }

            removedIDs.add(subVec.value_at(i).mMessageId);
            const uint32_t last = subVec.size() - 1u;
            if (i != last)
                subVec.set_value_at(i, subVec.value_at(last));
            subVec.remove_at(last);
        }
    }
}

void StubBase::clear_all_listeners( const ProxyAddress & whichProxy ) noexcept
{
    for (StubListenerMap::MAPPOS mapPos = mListenerMap.first_position(); mListenerMap.is_valid_position(mapPos); mapPos = mListenerMap.next_position(mapPos))
    {
        StubListenerList& subVec = mapPos->second;
        uint32_t i = 0;
        while (i < subVec.size())
        {
            if (subVec.value_at(i).mProxy != whichProxy)
            {
                ++i;
                continue;
            }

            const uint32_t last = subVec.size() - 1u;
            if (i != last)
                subVec.set_value_at(i, subVec.value_at(last));
            subVec.remove_at(last);
        }
    }
}

void StubBase::remove_from_map( uint32_t msgId, const StubBase::Listener & toRemove ) noexcept
{
    StubListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
        return;

    StubListenerList & subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if (subVec.value_at(i) == toRemove)
        {
            uint32_t last = subVec.size() - 1;
            if (i != last)
                subVec.set_value_at(i, subVec.value_at(last));
            subVec.remove_at(last);
            return;
        }
    }
}

void StubBase::send_response_notification( const StubListenerList & whichListeners, ServiceResponseEvent & masterEvent )
{
    const uint32_t count = whichListeners.size();
    if (count == 0)
        return;

    const uint32_t respId = masterEvent.response_id();

    // Clone for listeners [1..count-1] first while masterEvent is still alive.
    for (uint32_t i = 1; i < count; ++i)
    {
        const StubBase::Listener & listener = whichListeners.value_at(i);
        ServiceResponseEvent eventClone = masterEvent.clone_for_target(listener.mProxy);
        if (eventClone.is_valid())
        {
            if (static_cast<SignedSequence>(listener.mSequenceNr) >= 0)
            {
                eventClone.set_sequence_number(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    remove_from_map(respId, listener);
            }
            else
            {
                eventClone.set_sequence_number(static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(listener.mSequenceNr)));
                remove_from_map(respId, StubBase::Listener(respId, 0, listener.mProxy));
            }
            send_service_response(eventClone);
        }
    }

    // Send masterEvent directly to listener[0] — no clone, transfers ownership to dispatcher.
    const StubBase::Listener & first = whichListeners.value_at(0);
    masterEvent.set_sequence_number(static_cast<int32_t>(first.mSequenceNr) >= 0
            ? first.mSequenceNr
            : static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(first.mSequenceNr))
    );

    if (static_cast<int32_t>(first.mSequenceNr) >= 0)
    {
        if (first.mSequenceNr != 0)
            remove_from_map(respId, first);
    }
    else
    {
        remove_from_map(respId, StubBase::Listener(respId, 0, first.mProxy));
    }

    send_service_response(masterEvent);
}

void StubBase::send_error_notification( const StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent )
{
    const uint32_t respId = masterEvent.response_id();
    for (uint32_t i = 0; i < whichListeners.size(); ++i)
    {
        const StubBase::Listener & listener = whichListeners.value_at(i);
        ServiceResponseEvent eventError = masterEvent.clone_for_target(listener.mProxy);
        if (eventError.is_valid())
        {
            if (static_cast<int32_t>(listener.mSequenceNr) >= 0)
            {
                eventError.set_sequence_number(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    remove_from_map(respId, listener);
            }
            else
            {
                eventError.set_sequence_number(static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(listener.mSequenceNr)));
            }

            send_service_response(eventError);
        }
    }
}

void StubBase::send_update_notification( const StubListenerList & whichListeners, ServiceResponseEvent & masterEvent ) const
{
    const uint32_t count = whichListeners.size();
    if (count == 0)
        return;

    // Clone for listeners [1..count-1] first while masterEvent is still alive.
    for (uint32_t i = 1; i < count; ++i)
    {
        const StubBase::Listener & listener = whichListeners.value_at(i);
        ServiceResponseEvent eventClone = masterEvent.clone_for_target(listener.mProxy);
        if (eventClone.is_valid())
        {
            send_service_response(eventClone);
        }
    }

    // Send masterEvent directly to listener[0] — transfers ownership to dispatcher.
    send_service_response(masterEvent);
}

void StubBase::send_service_response( ServiceResponseEvent & eventElem ) const
{
    eventElem.deliver_event();
}

void StubBase::cancel_current_request() noexcept
{
    mCurrMsgId = INVALID_MESSAGE_ID;
}

ComponentThread & StubBase::component_thread() const noexcept
{
    return mComponent.master_thread();
}

void StubBase::startup_service_interface( Component&  holder )
{
    LOG_SCOPE( areg_component_StubBase, startup_service_interface );
    LOG_DBG( "Service with role [ %s ] and interface [ %s ] is started", service_role( ).as_string( ), service_name( ).as_string( ) );

    StubConnectEvent::add_listener( static_cast<StubEventConsumer &>(self()), holder.master_thread() );
}

void StubBase::shutdown_service_interface( Component & holder ) noexcept
{
    LOG_SCOPE( areg_component_StubBase, shutdown_service_intrface );
    LOG_INFO( "Service with role [ %s ] and interface [ %s ] is stopped", service_role().as_string(), service_name().as_string() );
    StubConnectEvent::remove_listener( static_cast<StubEventConsumer &>(self()), holder.master_thread() );
}

void StubBase::error_all_requests()
{
    LOG_SCOPE( areg_component_StubBase, error_all_requests );
    LOG_INFO( "Service [ %s ] with interface [ %s ] send errors to all consumer.", service_role().as_string(), service_name().as_string() );

    uint32_t i;

    const uint32_t numOfAttr= number_of_attributes();
    const uint32_t numOfResp= number_of_responses();
    const uint32_t numOfReqs= number_of_requests();
    const uint32_t* attrIds = attribute_ids();
    const uint32_t* respIds = response_ids();
    const uint32_t* reqIds  = request_ids();

    ASSERT(attrIds != nullptr || numOfAttr == 0);
    ASSERT(respIds != nullptr || numOfResp == 0);
    ASSERT(reqIds  != nullptr || numOfReqs == 0);

    for (i = 0; i < numOfAttr; ++i)
    {
        error_request(attrIds[i], false);
    }

    for (i = 0; i < numOfResp; ++i)
    {
        error_request(respIds[i], false);
    }

    for (i = 0; i < numOfReqs; ++i)
    {
        error_request(reqIds[i], false);
    }
}

void StubBase::cancel_all_requests()
{
    const uint32_t numOfReqs= number_of_requests();
    const uint32_t* reqIds  = request_ids();
    for ( uint32_t i = 0; i < numOfReqs; ++ i )
    {
        error_request( reqIds[i], true );
    }
}

void StubBase::invalidate_attribute( uint32_t attrId )
{
    if ( areg::is_attribute_id(attrId) )
        error_request(attrId, false);
}

void StubBase::send_update_event( uint32_t msgId, const SharedBuffer & data, areg::ResultType result ) const
{
    LOG_SCOPE( areg_component_StubBase, send_update_event );
    StubBase::StubListenerList listeners;
    if (find_listeners(msgId, listeners) > 0)
    {
        const ProxyAddress & proxy = listeners.first_entry().mProxy;
        LOG_WARN( "Sends busy message to proxy [ %s ] for the request [ %u ]", ProxyAddress::to_path( proxy).as_string(), msgId);

        ServiceResponseEvent eventElem = create_response(proxy, msgId, result, data);
        if (eventElem.is_valid())
        {
            send_update_notification(listeners, eventElem);
        }
    }
}

void StubBase::send_notify_once( const ProxyAddress & target, uint32_t msgId, const SharedBuffer & data, areg::ResultType result ) const
{
    ServiceResponseEvent eventElem = create_response( target, msgId, result, data );
    if ( eventElem.is_valid() )
    {
        send_service_response( eventElem );
    }
}

void StubBase::send_response_event( uint32_t respId, const SharedBuffer & data )
{
    StubBase::StubListenerList listeners;
    if (find_listeners(respId, listeners) > 0)
    {
        ServiceResponseEvent eventElem = create_response(listeners.first_entry().mProxy, respId, areg::ResultType::RequestOK, data);
        if (eventElem.is_valid())
        {
            send_response_notification(listeners, eventElem);
        }
    }
}

void StubBase::send_busy_response( const Listener & whichListener )
{
    LOG_SCOPE( areg_component_StubBase, send_busy_response );
    ServiceResponseEvent eventElem = create_response(whichListener.mProxy, whichListener.mMessageId, areg::ResultType::RequestBusy, SharedBuffer{});
    if (eventElem.is_valid())
    {
        LOG_WARN("Sending busy response for request message [ %p ] from source [ %p ] to target [ %p ], sequence [ %llu ]"
                    , whichListener.mMessageId
                    , whichListener.mProxy.target()
                    , whichListener.mProxy.source()
                    , whichListener.mSequenceNr);

        eventElem.set_sequence_number(whichListener.mSequenceNr);
        send_service_response(eventElem);
    }
}

bool StubBase::can_execute_request( Listener & whichListener, uint32_t whichResponse, const SequenceNumber & seqNr )
{
    bool result = false;
    if (is_busy(whichResponse))
    {
        whichListener.mSequenceNr   = seqNr;
        send_busy_response(whichListener);
    }
    else
    {
        prepare_request(whichListener, seqNr, whichResponse);
        result = true;
    }

    return result;
}

bool StubBase::exist( uint32_t msgId, const ProxyAddress & notifySource ) const noexcept
{
    if (!notifySource.is_valid())
        return false;

    StubListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
        return false;

    const StubListenerList & subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        const StubBase::Listener & listener = subVec.value_at(i);
        if (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && notifySource == listener.mProxy)
            return true;
    }

    return false;
}

bool StubBase::add_notification_listener(uint32_t msgId, const ProxyAddress & notifySource)
{
    LOG_SCOPE( areg_component_StubBase, add_notification_listener );

    bool result { false };
    if (notifySource.is_valid())
    {
        StubListenerList & subVec = mListenerMap[msgId];
        bool hasEntry{ false };
        for (uint32_t i = 0; i < subVec.size(); ++i)
        {
            const StubBase::Listener & listener = subVec.value_at(i);
            if (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && notifySource == listener.mProxy)
            {
                hasEntry = true;
                break;
            }
        }

        if (!hasEntry)
        {
            LOG_DBG("For the message [ %u ] new listener [ %s ] is added"
                        , msgId
                        , ProxyAddress::to_path(notifySource).as_string());

            subVec.add(StubBase::Listener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, notifySource));
            result = true;
        }
#if AREG_LOGGING
        else
        {
            LOG_WARN("For the message [ %u ] there is already registered client [ %s ]"
                       , msgId
                       , ProxyAddress::to_path(notifySource).as_string());
        }
#endif  // AREG_LOGGING
    }

    return result;
}

void StubBase::remove_notification_listener( uint32_t msgId, const ProxyAddress & notifySource ) noexcept
{
    StubListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
        return;

    StubListenerList & subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        const StubBase::Listener & listener = subVec.value_at(i);
        if (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && notifySource == listener.mProxy)
        {
            uint32_t last = subVec.size() - 1;
            if (i != last)
                subVec.set_value_at(i, subVec.value_at(last));
            subVec.remove_at(last);
            break;
        }
    }
}

bool StubBase::consumer_connected(const ProxyAddress & client, areg::ServiceConnectionState status )
{
    bool result{ false };
    if (mAddress == client)
    {
        LOG_SCOPE( areg_component_StubBase, consumer_connected);
        LOG_DBG("Service consumer [ %s ] connection event with status [ %s ]"
                  , ProxyAddress::to_path(client).as_string()
                  , areg::as_string(status));

        result = true;
        if (areg::is_service_disconnected(status))
        {
            clear_all_listeners(client);
        }
    }

    return result;
}

void StubBase::process_connect_event( const ProxyAddress & proxyAddress, areg::ServiceConnectionState status )
{
    consumer_connected( proxyAddress, status );
}

void StubBase::process_registered_event(const StubAddress & stubTarget, areg::ServiceConnectionState status )
{
    if ( areg::is_service_connected( status) )
    {
        ASSERT( stubTarget.is_valid() );
        map_providers().lock();
        map_providers().unregister_resource_object(static_cast<uint32_t>(mAddress));

        mAddress = stubTarget;
        
        map_providers().register_resource_object(static_cast<uint32_t>(mAddress), this);
        map_providers().unlock();
    }

    mConnectionStatus = status;
}

const Version & StubBase::impl_version() const noexcept
{
    return mInterface.idVersion;
}

uint32_t StubBase::number_of_requests() const noexcept
{
    return mInterface.idRequestCount;
}

uint32_t StubBase::number_of_responses() const noexcept
{
    return mInterface.idResponseCount;
}

uint32_t StubBase::number_of_attributes() const noexcept
{
    return mInterface.idAttributeCount;
}

const uint32_t * StubBase::request_ids() const noexcept
{
    return mInterface.idRequestList;
}

const uint32_t * StubBase::response_ids() const noexcept
{
    return mInterface.idResponseList;
}

const uint32_t * StubBase::attribute_ids() const noexcept
{
    return mInterface.idAttributeList;
}

ServiceResponseEvent StubBase::create_response( const ProxyAddress &    /* proxy  */
                                              , uint32_t               /* msgId  */
                                              , areg::ResultType       /* result */
                                              , const SharedBuffer &   /* data   */ ) const
{
    return ServiceResponseEvent(EventEnvelope{});  // invalid; derived stubs override to produce real events
}

ServiceResponseEvent StubBase::create_response_event( const ProxyAddress &    /* proxy  */
                                                    , uint32_t               /* msgId  */
                                                    , areg::ResultType       /* result */
                                                    , uint32_t               /* reserve */ ) const
{
    return ServiceResponseEvent(EventEnvelope{});  // invalid; derived stubs override to produce real events
}

ServiceResponseEvent StubBase::prepare_response_event( uint32_t respId, areg::ResultType result, uint32_t reserve, StubBase::StubListenerList & out_listeners )
{
    if (find_listeners(respId, out_listeners) > 0)
        return create_response_event(out_listeners.first_entry().mProxy, respId, result, reserve);

    return ServiceResponseEvent(EventEnvelope{});  // no listeners: invalid event
}

void StubBase::process_stub_event( StubEvent & /* eventElem */ )
{
}

void StubBase::process_generic_event(Event & /* eventElem */)
{
}

} // namespace areg
