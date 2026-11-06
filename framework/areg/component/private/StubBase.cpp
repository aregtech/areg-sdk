/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/component/private/ServiceManager.hpp"

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
DEF_LOG_SCOPE(areg_component_StubBase, collect_and_strip_response_listeners);

//////////////////////////////////////////////////////////////////////////
// StubBase::Listener implementation
//////////////////////////////////////////////////////////////////////////

bool StubBase::Listener::operator == ( const StubBase::Listener & other ) const noexcept
{
    if (this == &other)
        return true;
    
    ASSERT(mMessageId == other.mMessageId);
    return (areg::SEQUENCE_NUMBER_ANY == other.mSequenceNr) || ((mSequenceNr == other.mSequenceNr) && same_proxy(other));
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
    , mIsStarted            ( false )
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
    _leave_registry();
}

void StubBase::_leave_registry()
{
    // Removes the entry only if it is this provider.
    MapProviderResource & providers{ map_providers() };
    Lock lock(providers.lockable());
    if (providers.find_resource_object(static_cast<uint32_t>(mAddress)) == this)
    {
        providers.unregister_resource_object(static_cast<uint32_t>(mAddress));
    }
}

StubAddress StubBase::registered_address( const StubAddress & address )
{
    MapProviderResource & providers{ map_providers() };
    Lock lock(providers.lockable());
    const StubBase * stub{ providers.find_resource_object(static_cast<uint32_t>(address)) };
    return (stub != nullptr ? stub->mAddress : address);
}

void StubBase::_detach_thread_providers( const ArrayList<String> & threads )
{
    ArrayList<ThreadAddress> addresses;
    for (uint32_t i = 0; i < threads.size(); ++ i)
    {
        addresses.add(ThreadAddress(threads[i]));
    }

    MapProviderResource & providers{ map_providers() };
    Lock lock(providers.lockable());
    ArrayList<uint32_t> keys;
    for (const auto & entry : providers.data())
    {
        if (addresses.contains(entry.second->mAddress.thread()))
        {
            keys.add(entry.first);
        }
    }

    for (uint32_t i = 0; i < keys.size(); ++ i)
    {
        providers.unregister_resource_object(keys[i]);
    }
}

bool StubBase::is_busy( uint32_t reqId ) const noexcept
{
    Lock lock(mListenerLock);
    StubListenerMap::MAPPOS pos = mListenerMap.find(reqId);
    if (!mListenerMap.is_valid_position(pos))
        return false;

    const StubListenerList& subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if (subVec.value_at(i).is_request())
            return true;
    }

    return false;
}

SessionID StubBase::unblock_current_request()
{
    SessionID result = StubBase::INVALID_SESSION_ID;
    if (mCurrMsgId != INVALID_MESSAGE_ID)
    {
        Lock lock(mListenerLock);
        StubListenerMap::MAPPOS pos = mListenerMap.find(mCurrMsgId);
        if (mListenerMap.is_valid_position(pos))
        {
            StubListenerList& subVec = pos->second;
            const uint32_t index{ _find_unprepared(subVec, mCurrIndex) };
            if (index < subVec.size())
            {
                const Listener listener{ subVec.value_at(index) };
                _remove_at(subVec, index);
                do
                {
                    result = ++mSessionId;
                } while ((result == StubBase::INVALID_SESSION_ID) || (mMapSessions.add_if_unique(result, listener).second == false));
            }
        }

        mCurrMsgId = INVALID_MESSAGE_ID;
    }

    return result;
}

bool StubBase::prepare_response( SessionID sessionId )
{
    Lock lock(mListenerLock);
    MapProviderSession::MAPPOS pos = mMapSessions.find(sessionId);
    if (!mMapSessions.is_valid_position(pos))
        return false;

    StubListenerList& subVec = mListenerMap[pos->second.mMessageId];
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if ((subVec.value_at(i).mFlags & Listener::FLAG_PREPARED) != 0u)
            return false;
    }

    Listener listener{ pos->second };
    listener.mFlags |= Listener::FLAG_PREPARED;
    subVec.add(listener);
    static_cast<void>(mMapSessions.remove_at(pos));
    return true;
}

void StubBase::prepare_request( Listener & listener, const SequenceNumber & seqNr, uint32_t respId )
{
    Lock lock(mListenerLock);

    listener.mMessageId = respId;
    listener.mSequenceNr= seqNr;
    listener.mFlags     = 0u;
    StubListenerList& subVec = mListenerMap[respId];
    subVec.add(listener);
    mCurrMsgId = respId;
    mCurrIndex  = subVec.size() - 1;
}

uint32_t StubBase::find_listeners( uint32_t reqId, StubListenerList & listeners ) const
{
    Lock lock(mListenerLock);
    StubListenerMap::MAPPOS pos = mListenerMap.find(reqId);
    if (mListenerMap.is_valid_position(pos))
        listeners.append(pos->second);
    return listeners.size();
}

void StubBase::clear_all_listeners( const ProxyAddress & whichProxy, IntegerArray & removedIDs )
{
    Lock lock(mListenerLock);
    for (StubListenerMap::MAPPOS mapPos = mListenerMap.first_position(); mListenerMap.is_valid_position(mapPos); mapPos = mListenerMap.next_position(mapPos))
    {
        StubListenerList& subVec = mapPos->second;
        uint32_t i = 0;
        while (i < subVec.size())
        {
            StubBase::Listener& listener{ subVec[i] };
            if (!listener.proxy_matches(whichProxy))
            {
                ++i;
            }
            else if (listener.is_request())
            {
                listener.mFlags |= Listener::FLAG_ORPHANED;
                ++i;
            }
            else
            {
                removedIDs.add(listener.mMessageId);
                _remove_at(subVec, i);
            }
        }
    }

    for (MapProviderSession::MAPPOS pos = mMapSessions.first_position(); mMapSessions.is_valid_position(pos); pos = mMapSessions.next_position(pos))
    {
        if (pos->second.proxy_matches(whichProxy))
            pos->second.mFlags |= Listener::FLAG_ORPHANED;
    }
}

void StubBase::clear_all_listeners( const ProxyAddress & whichProxy ) noexcept
{
    Lock lock(mListenerLock);
    for (StubListenerMap::MAPPOS mapPos = mListenerMap.first_position(); mListenerMap.is_valid_position(mapPos); mapPos = mListenerMap.next_position(mapPos))
    {
        StubListenerList& subVec = mapPos->second;
        uint32_t i = 0;
        while (i < subVec.size())
        {
            StubBase::Listener& listener{ subVec[i] };
            if (!listener.proxy_matches(whichProxy))
            {
                ++i;
            }
            else if (listener.is_request())
            {
                listener.mFlags |= Listener::FLAG_ORPHANED;
                ++i;
            }
            else
            {
                _remove_at(subVec, i);
            }
        }
    }

    for (MapProviderSession::MAPPOS pos = mMapSessions.first_position(); mMapSessions.is_valid_position(pos); pos = mMapSessions.next_position(pos))
    {
        if (pos->second.proxy_matches(whichProxy))
            pos->second.mFlags |= Listener::FLAG_ORPHANED;
    }
}

void StubBase::_remove_at( StubListenerList & subVec, uint32_t index ) noexcept
{
    const uint32_t last = subVec.size() - 1u;
    if (index != last)
        subVec.set_value_at(index, subVec.value_at(last));
    subVec.remove_at(last);
}

uint32_t StubBase::_find_target( const StubListenerList & subVec ) noexcept
{
    uint32_t result{ subVec.size() };
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        const Listener & listener{ subVec.value_at(i) };
        if (!listener.is_request())
            continue;
        if ((listener.mFlags & Listener::FLAG_PREPARED) != 0u)
            return i;
        if (result == subVec.size())
            result = i;
    }

    return result;
}

uint32_t StubBase::_find_unprepared( const StubListenerList & subVec, uint32_t hint ) noexcept
{
    const auto isUnprepared = [](const Listener & listener) noexcept -> bool
        {
            return listener.is_request() && ((listener.mFlags & Listener::FLAG_PREPARED) == 0u);
        };

    if ((hint < subVec.size()) && isUnprepared(subVec.value_at(hint)))
        return hint;

    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if (isUnprepared(subVec.value_at(i)))
            return i;
    }

    return subVec.size();
}

uint32_t StubBase::_request_of( uint32_t respId ) const noexcept
{
    const uint32_t * map{ mInterface.idRequestToResponseMap };
    for (uint32_t i = 0; (map != nullptr) && (i < mInterface.idRequestCount); ++i)
    {
        if (map[i] == respId)
            return mInterface.idRequestList[i];
    }

    return INVALID_MESSAGE_ID;
}

void StubBase::_cancel_sessions( uint32_t reqId, uint32_t respId )
{
    StubListenerList cancelled;
    {
        Lock lock(mListenerLock);
        for (MapProviderSession::MAPPOS pos = mMapSessions.first_position(); mMapSessions.is_valid_position(pos); )
        {
            if (pos->second.mMessageId == respId)
            {
                cancelled.add(pos->second);
                pos = mMapSessions.remove_at(pos);
            }
            else
            {
                pos = mMapSessions.next_position(pos);
            }
        }
    }

    for (uint32_t i = 0; i < cancelled.size(); ++i)
    {
        const Listener & listener{ cancelled.value_at(i) };
        if ((listener.mFlags & Listener::FLAG_ORPHANED) != 0u)
            continue;

        ServiceResponseEvent eventError{ create_response(listener.proxy(), reqId, areg::ResultType::RequestCanceled, SharedBuffer{}) };
        if (eventError.is_valid())
        {
            eventError.set_sequence_number(listener.mSequenceNr);
            send_service_response(eventError);
        }
    }
}

void StubBase::collect_and_strip_response_listeners( uint32_t respId, StubListenerList & listeners )
{
    bool dropped{ false };
    do
    {
        Lock lock(mListenerLock);
        StubListenerMap::MAPPOS pos = mListenerMap.find(respId);
        if (!mListenerMap.is_valid_position(pos))
            break;

        StubListenerList & subVec = pos->second;
        const uint32_t index{ _find_target(subVec) };
        if (index < subVec.size())
        {
            const Listener target{ subVec.value_at(index) };
            _remove_at(subVec, index);
            const bool reachable{ (target.mFlags & Listener::FLAG_ORPHANED) == 0u };
            listeners.reserve(subVec.size() + 1u);
            if (reachable)
                listeners.add(target);

            for (uint32_t i = 0; i < subVec.size(); ++i)
            {
                const Listener & listener{ subVec.value_at(i) };
                if (!listener.is_request() && (!reachable || !listener.same_proxy(target)))
                    listeners.add(listener);
            }
        }
        else if (_request_of(respId) != INVALID_MESSAGE_ID)
        {
            dropped = true;
        }
        else
        {
            listeners.append(subVec);
        }
    } while (false);

    if (dropped)
    {
        LOG_SCOPE(areg_component_StubBase, collect_and_strip_response_listeners);
        LOG_WARN("Response [ %u ] of service [ %s ] has no waiting request, it is dropped", respId, mAddress.service_name().as_string());
    }
}

void StubBase::send_response_notification( const StubListenerList & whichListeners, ServiceResponseEvent & masterEvent )
{
    const uint32_t count = whichListeners.size();
    if ((count == 0) || (masterEvent.is_valid() == false))
        return;

    // Clone for listeners [1..count-1] first while masterEvent is still alive.
    for (uint32_t i = 1; i < count; ++i)
    {
        const StubBase::Listener & listener = whichListeners.value_at(i);
        ServiceResponseEvent eventClone{ masterEvent.clone_for_target(listener.mConsumer, listener.mRawService) };
        if (eventClone.is_valid())
        {
            eventClone.set_sequence_number(listener.mSequenceNr);
            send_service_response(eventClone);
        }
    }

    // Send masterEvent directly to listener[0] -- no clone, transfers ownership to dispatcher.
    const StubBase::Listener & first = whichListeners.value_at(0);
    masterEvent.set_sequence_number(first.mSequenceNr);
    send_service_response(masterEvent);
}

void StubBase::send_error_notification( const StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent )
{
    const uint32_t msgId{ masterEvent.response_id() };
    const uint32_t listId{ whichListeners.is_empty() ? msgId : whichListeners.value_at(0).mMessageId };
    const bool isRequest{ areg::is_request_id(msgId) };
    const bool isResponse{ areg::is_response_id(msgId) };
    const bool cancelAll{ isRequest && (static_cast<areg::ResultType>(masterEvent.envelope().result()) == areg::ResultType::RequestCanceled) };

    StubListenerList targets;
    do
    {
        Lock lock(mListenerLock);
        StubListenerMap::MAPPOS pos = mListenerMap.find(listId);
        if (!mListenerMap.is_valid_position(pos))
            break;

        StubListenerList & subVec = pos->second;
        if (isRequest && !cancelAll)
        {
            const uint32_t index{ _find_target(subVec) };
            if (index < subVec.size())
            {
                targets.add(subVec.value_at(index));
                _remove_at(subVec, index);
            }
        }
        else if (isRequest || isResponse)
        {
            uint32_t i = 0;
            while (i < subVec.size())
            {
                if (subVec.value_at(i).is_request())
                {
                    targets.add(subVec.value_at(i));
                    _remove_at(subVec, i);
                }
                else
                {
                    ++i;
                }
            }
        }
        else
        {
            targets.append(subVec);
        }
    } while (false);

    const uint32_t reqId{ isResponse ? _request_of(listId) : msgId };
    for (uint32_t i = 0; i < targets.size(); ++i)
    {
        const StubBase::Listener & listener = targets.value_at(i);
        if ((listener.mFlags & Listener::FLAG_ORPHANED) != 0u)
            continue;

        ServiceResponseEvent eventError{ isResponse
                                       ? create_response(listener.proxy(), reqId, areg::ResultType::RequestError, SharedBuffer{})
                                       : masterEvent.clone_for_target(listener.mConsumer, listener.mRawService) };
        if (eventError.is_valid())
        {
            eventError.set_sequence_number(listener.mSequenceNr);
            send_service_response(eventError);
        }
    }

    if (cancelAll)
    {
        _cancel_sessions(msgId, listId);
    }
}

void StubBase::send_update_notification( const StubListenerList & whichListeners, ServiceResponseEvent & masterEvent ) const
{
    const uint32_t count = whichListeners.size();
    if ((count == 0) || (masterEvent.is_valid() == false))
        return;

    for (uint32_t i = 1; i < count; ++i)
    {
        const StubBase::Listener & listener = whichListeners.value_at(i);
        ServiceResponseEvent eventClone{ masterEvent.clone_for_target(listener.mConsumer, listener.mRawService) };
        if (eventClone.is_valid())
        {
            send_service_response(eventClone);
        }
    }

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

    mIsStarted = true;
    StubConnectEvent::add_listener( static_cast<StubEventConsumer &>(self()), holder.master_thread() );
}

void StubBase::shutdown_service_interface( Component & holder ) noexcept
{
    LOG_SCOPE( areg_component_StubBase, shutdown_service_intrface );
    LOG_INFO( "Service with role [ %s ] and interface [ %s ] is stopped", service_role().as_string(), service_name().as_string() );
    mIsStarted = false;
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
    const uint32_t* respIds = mInterface.idRequestToResponseMap;
    for ( uint32_t i = 0; i < numOfReqs; ++ i )
    {
        error_request( reqIds[i], true );
        if ((respIds != nullptr) && areg::is_valid_response_id(respIds[i]))
        {
            _cancel_sessions(reqIds[i], respIds[i]);
        }
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
        const ProxyAddress proxy{ listeners.first_entry().proxy() };
        LOG_DBG( "Sending update of message [ %u ] with result [ %s ] to [ %u ] subscribed consumer(s), first is [ %s ]"
                   , msgId
                   , areg::as_string( result )
                   , listeners.size()
                   , ProxyAddress::to_path( proxy ).as_string() );

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
    collect_and_strip_response_listeners(respId, listeners);
    if (listeners.size() > 0)
    {
        ServiceResponseEvent eventElem = create_response(listeners.first_entry().proxy(), respId, areg::ResultType::RequestOK, data);
        if (eventElem.is_valid())
        {
            send_response_notification(listeners, eventElem);
        }
    }
}

void StubBase::send_busy_response( const Listener & whichListener )
{
    LOG_SCOPE( areg_component_StubBase, send_busy_response );
    const ProxyAddress proxy{ whichListener.proxy() };
    ServiceResponseEvent eventElem = create_response(proxy, whichListener.mMessageId, areg::ResultType::RequestBusy, SharedBuffer{});
    if (eventElem.is_valid())
    {
        LOG_WARN("Sending busy response for request message [ %u ] from source [ %u ] to target [ %u ], sequence [ %llu ]"
                    , whichListener.mMessageId
                    , static_cast<uint32_t>(proxy.target())
                    , static_cast<uint32_t>(proxy.source())
                    , static_cast<uint64_t>(whichListener.mSequenceNr));

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

    Lock lock(mListenerLock);
    StubListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
        return false;

    const StubListenerList & subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        const StubBase::Listener & listener = subVec.value_at(i);
        if (listener.proxy_matches(notifySource) && (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr))
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
        Lock lock(mListenerLock);
        StubListenerList & subVec = mListenerMap[msgId];
        bool hasEntry{ false };
        for (uint32_t i = 0; i < subVec.size(); ++i)
        {
            const StubBase::Listener & listener = subVec.value_at(i);
            if (listener.proxy_matches(notifySource) && (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr))
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
    Lock lock(mListenerLock);
    StubListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
        return;

    StubListenerList & subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        const StubBase::Listener & listener = subVec.value_at(i);
        if (listener.proxy_matches(notifySource) && (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr))
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

bool StubBase::can_process_requests( ) const
{
    return mIsStarted;
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
                                              , uint32_t                /* msgId  */
                                              , areg::ResultType        /* result */
                                              , const SharedBuffer &    /* data   */ ) const
{
    return ServiceResponseEvent(MessageEnvelope{});  // invalid; derived stubs override to produce real events
}

ServiceResponseEvent StubBase::create_response_event( const ProxyAddress &  /* proxy  */
                                                    , uint32_t              /* msgId  */
                                                    , areg::ResultType      /* result */
                                                    , uint32_t              /* reserve */ ) const
{
    return ServiceResponseEvent(MessageEnvelope{});  // invalid; derived stubs override to produce real events
}

ServiceResponseEvent StubBase::create_undelivered_response( const ProxyAddress & proxy, uint32_t msgId, const SequenceNumber & seqNr ) const
{
    if (!areg::is_request_id(msgId))
        return ServiceResponseEvent(MessageEnvelope{});

    ServiceResponseEvent failure{ create_response(proxy, msgId, areg::ResultType::MessageUndelivered, SharedBuffer{}) };
    if (failure.is_valid())
    {
        failure.set_sequence_number(seqNr);
    }

    return failure;
}

ServiceResponseEvent StubBase::prepare_response_event( uint32_t respId, areg::ResultType result, uint32_t reserve, StubBase::StubListenerList & listeners )
{
    collect_and_strip_response_listeners(respId, listeners);
    if (listeners.size() > 0)
        return create_response_event(listeners.first_entry().proxy(), respId, result, reserve);

    return ServiceResponseEvent(MessageEnvelope{});  // no listeners: invalid event
}

void StubBase::process_stub_event( StubEvent & /* eventElem */ )
{
}

void StubBase::process_generic_event(Event & /* eventElem */)
{
}

} // namespace areg
