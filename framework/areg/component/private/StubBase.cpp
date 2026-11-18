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
#include "areg/base/private/NeverDestroyed.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// StubBase class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_StubBase, startup_service_interface);
DEF_LOG_SCOPE(areg_component_StubBase, _enter_registry);
DEF_LOG_SCOPE(areg_component_StubBase, shutdown_service_intrface);
DEF_LOG_SCOPE(areg_component_StubBase, error_all_requests);
DEF_LOG_SCOPE(areg_component_StubBase, send_update_event);
DEF_LOG_SCOPE(areg_component_StubBase, send_busy_response);
DEF_LOG_SCOPE(areg_component_StubBase, consumer_connected);
DEF_LOG_SCOPE(areg_component_StubBase, add_notification_listener);
DEF_LOG_SCOPE(areg_component_StubBase, collect_and_strip_response_listeners);
DEF_LOG_SCOPE(areg_component_StubBase, can_execute_request);

//////////////////////////////////////////////////////////////////////////
// StubBase::Listener implementation
//////////////////////////////////////////////////////////////////////////

bool StubBase::Listener::operator == ( const StubBase::Listener & other ) const noexcept
{
    if (this == &other)
        return true;

    return (areg::SEQUENCE_NUMBER_ANY == other.mSequenceNr) || ((mSequenceNr == other.mSequenceNr) && same_proxy(other));
}

//////////////////////////////////////////////////////////////////////////
// StubBase implementation
//////////////////////////////////////////////////////////////////////////

StubBase::MapProviderResource& StubBase::map_providers() noexcept
{
    static NeverDestroyed<StubBase::MapProviderResource> _mapProviders;
    return _mapProviders.object();
}

StubBase::StubBase( Component & masterComp, const areg::InterfaceData & siData )
    : StubEventConsumer   ( mAddress )

    , mComponent            (masterComp)
    , mInterface            (siData)
    , mAddress              (siData, masterComp.address().role_name(), masterComp.address().thread_address().name())
    , mConnectionStatus     ( areg::ServiceConnectionState::Disconnected )
    , mIsStarted            ( false )
    , mListeners            ( areg::listener_slot_count(siData) )
    , mCurrSlot             ( INVALID_SLOT )
    , mCurrIndex            ( 0 )
    , mMapSessions          ( )
    , mSessionId            (0)
{
    _enter_registry();
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

void StubBase::_enter_registry()
{
    bool refused{ false };
    StubAddress holder{ StubAddress::invalid_stub_address() };
    {
        MapProviderResource & providers{ map_providers() };
        Lock lock(providers.lockable());
        const StubBase * stub{ providers.find_resource_object(static_cast<uint32_t>(mAddress)) };
        if ((stub != nullptr) && (stub->mAddress.has_name_conflict(mAddress) || (stub->mAddress.thread().name() != mAddress.thread().name())))
        {
            refused = true;
            holder  = stub->mAddress;
        }
        else
        {
            providers.register_resource_object(static_cast<uint32_t>(mAddress), this);
        }
    }

    if (refused)
    {
        LOG_SCOPE(areg_component_StubBase, _enter_registry);
        LOG_ERR("The provider [ %s ] is refused: its number is used by the provider [ %s ], rename one of them"
                    , StubAddress::to_path(mAddress).as_string()
                    , StubAddress::to_path(holder).as_string());
    }
}

StubAddress StubBase::registered_address( const StubAddress & address )
{
    MapProviderResource & providers{ map_providers() };
    Lock lock(providers.lockable());
    const StubBase * stub{ providers.find_resource_object(static_cast<uint32_t>(address)) };
    return (stub != nullptr ? stub->mAddress : address);
}

void StubBase::_detach_thread_providers( const ThreadAddress & thread )
{
    MapProviderResource & providers{ map_providers() };
    Lock lock(providers.lockable());
    for (bool found{ true }; found; )
    {
        found = false;
        uint32_t key{ 0u };
        for (const auto & entry : providers.data())
        {
            if (entry.second->mAddress.thread() == thread)
            {
                key = entry.first;
                found = true;
                break;
            }
        }

        if (found)
        {
            providers.unregister_resource_object(key);
        }
    }
}

bool StubBase::is_busy( uint32_t respIndex ) const noexcept
{
    Lock lock(mListenerLock);
    const ListenerSlotList * subVec{ mListeners.at(_slot(areg::CallType::Response, respIndex)) };
    if (subVec == nullptr)
        return false;

    for (uint32_t i = 0; i < subVec->size(); ++i)
    {
        if (subVec->value_at(i).is_request())
            return true;
    }

    return false;
}

SessionID StubBase::unblock_current_request()
{
    SessionID result = StubBase::INVALID_SESSION_ID;
    if (mCurrSlot != INVALID_SLOT)
    {
        Lock lock(mListenerLock);
        ListenerSlotList * subVec{ mListeners.at(mCurrSlot) };
        if (subVec != nullptr)
        {
            const uint32_t index{ _find_unprepared(*subVec, mCurrIndex) };
            if (index < subVec->size())
            {
                const Listener listener{ subVec->value_at(index) };
                _remove_at(*subVec, index);
                do
                {
                    result = ++mSessionId;
                } while ((result == StubBase::INVALID_SESSION_ID) || (mMapSessions.add_if_unique(result, listener).second == false));
            }
        }

        mCurrSlot = INVALID_SLOT;
    }

    return result;
}

bool StubBase::prepare_response( SessionID sessionId )
{
    Lock lock(mListenerLock);
    MapProviderSession::MAPPOS pos = mMapSessions.find(sessionId);
    if (!mMapSessions.is_valid_position(pos))
        return false;

    ListenerSlotList * subVec{ mListeners.at(pos->second.mSlot) };
    if (subVec == nullptr)
        return false;

    for (uint32_t i = 0; i < subVec->size(); ++i)
    {
        if ((subVec->value_at(i).mFlags & Listener::FLAG_PREPARED) != 0u)
            return false;
    }

    Listener listener{ pos->second };
    listener.mFlags = static_cast<uint16_t>(listener.mFlags | Listener::FLAG_PREPARED);
    subVec->add(listener);
    static_cast<void>(mMapSessions.remove_at(pos));
    return true;
}

void StubBase::prepare_request( Listener & listener, const SequenceNumber & seqNr, uint32_t respIndex )
{
    Lock lock(mListenerLock);
    const uint32_t slot{ _slot(areg::CallType::Response, respIndex) };
    ListenerSlotList * subVec{ mListeners.at(slot) };
    if (subVec == nullptr)
        return;

    listener.mSlot      = static_cast<uint16_t>(slot);
    listener.mSequenceNr= seqNr;
    listener.mFlags     = 0u;
    subVec->add(listener);
    mCurrSlot   = slot;
    mCurrIndex  = subVec->size() - 1;
}

uint32_t StubBase::find_listeners( areg::CallType kind, uint32_t index, StubListenerList & listeners ) const
{
    const uint32_t slot{ kind == areg::CallType::Request ? _slot(areg::CallType::Response, mInterface.response_index(index)) : _slot(kind, index) };
    Lock lock(mListenerLock);
    const ListenerSlotList * subVec{ mListeners.at(slot) };
    if (subVec != nullptr)
        static_cast<void>(listeners.append(*subVec));

    return listeners.size();
}

void StubBase::remove_notification_listeners( const ProxyAddress & notifySource ) noexcept
{
    Lock lock(mListenerLock);
    for (uint32_t slot = 0u; slot < mListeners.size(); ++slot)
    {
        ListenerSlotList& subVec = *mListeners.at(slot);
        uint32_t i = 0;
        while (i < subVec.size())
        {
            const StubBase::Listener& listener{ subVec.value_at(i) };
            if (listener.proxy_matches(notifySource) && (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr))
            {
                _remove_at(subVec, i);
            }
            else
            {
                ++i;
            }
        }
    }
}

void StubBase::clear_all_listeners( const ProxyAddress & whichProxy ) noexcept
{
    Lock lock(mListenerLock);
    for (uint32_t slot = 0u; slot < mListeners.size(); ++slot)
    {
        ListenerSlotList& subVec = *mListeners.at(slot);
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
                listener.mFlags = static_cast<uint16_t>(listener.mFlags | Listener::FLAG_ORPHANED);
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
            pos->second.mFlags = static_cast<uint16_t>(pos->second.mFlags | Listener::FLAG_ORPHANED);
    }
}

void StubBase::_remove_at( ListenerSlotList & subVec, uint32_t index ) noexcept
{
    const uint32_t last = subVec.size() - 1u;
    if (index != last)
        subVec.set_value_at(index, subVec.value_at(last));
    subVec.remove_at(last);
}

uint32_t StubBase::_find_target( const ListenerSlotList & subVec, bool anyRequest /*= true*/, uint32_t reqId /*= 0u*/ ) noexcept
{
    uint32_t result{ subVec.size() };
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        const Listener & listener{ subVec.value_at(i) };
        if (!listener.is_request() || (!anyRequest && (listener.mMessageId != reqId)))
            continue;
        if ((listener.mFlags & Listener::FLAG_PREPARED) != 0u)
            return i;
        if (result == subVec.size())
            result = i;
    }

    return result;
}

uint32_t StubBase::_find_unprepared( const ListenerSlotList & subVec, uint32_t hint ) noexcept
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

void StubBase::_cancel_sessions( uint32_t reqId, uint32_t respSlot )
{
    StubListenerList cancelled;
    {
        Lock lock(mListenerLock);
        for (MapProviderSession::MAPPOS pos = mMapSessions.first_position(); mMapSessions.is_valid_position(pos); )
        {
            if ((pos->second.mSlot == respSlot) && (pos->second.mMessageId == reqId))
            {
                pos = cancelled.add(pos->second) ? mMapSessions.remove_at(pos) : mMapSessions.next_position(pos);
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

        ServiceResponseEvent eventError{ create_response(listener.proxy(), reqId, areg::CallType::Request, areg::ResultType::RequestCanceled, SharedBuffer{}) };
        if (eventError.is_valid())
        {
            eventError.set_sequence_number(listener.mSequenceNr);
            send_service_response(eventError);
        }
    }
}

void StubBase::collect_and_strip_response_listeners( areg::CallType kind, uint32_t index, StubListenerList & listeners )
{
    bool dropped{ false };
    do
    {
        Lock lock(mListenerLock);
        ListenerSlotList * found{ mListeners.at(_slot(kind, index)) };
        if (found == nullptr)
            break;

        ListenerSlotList & subVec = *found;
        const uint32_t pos{ _find_target(subVec) };
        if (pos < subVec.size())
        {
            const Listener target{ subVec.value_at(pos) };
            const bool reachable{ (target.mFlags & Listener::FLAG_ORPHANED) == 0u };
            if (reachable && !listeners.add(target))
                break;

            _remove_at(subVec, pos);
            static_cast<void>(listeners.reserve(listeners.size() + subVec.size()));

            for (uint32_t i = 0; i < subVec.size(); ++i)
            {
                const Listener & listener{ subVec.value_at(i) };
                if (!listener.is_request() && (!reachable || !listener.same_proxy(target)))
                    listeners.add(listener);
            }
        }
        else if (kind == areg::CallType::Response)
        {
            dropped = true;
        }
        else
        {
            static_cast<void>(listeners.append(subVec));
        }
    } while (false);

    if (dropped)
    {
        LOG_SCOPE(areg_component_StubBase, collect_and_strip_response_listeners);
        LOG_WARN("Response [ %u ] of service [ %s ] has no waiting request, it is dropped", index, mAddress.service_name().as_string());
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
    const uint32_t listSlot{ whichListeners.is_empty() ? INVALID_SLOT : static_cast<uint32_t>(whichListeners.value_at(0).mSlot) };
    const areg::CallType kind{ masterEvent.call_kind() };
    const bool isRequest{ kind == areg::CallType::Request };
    const bool isResponse{ kind == areg::CallType::Response };
    const bool isBroadcast{ kind == areg::CallType::Broadcast };
    const bool cancelAll{ isRequest && (static_cast<areg::ResultType>(masterEvent.envelope().result()) == areg::ResultType::RequestCanceled) };

    StubListenerList targets;
    do
    {
        Lock lock(mListenerLock);
        ListenerSlotList * found{ mListeners.at(listSlot) };
        if (found == nullptr)
            break;

        ListenerSlotList & subVec = *found;
        if (isRequest && !cancelAll)
        {
            const uint32_t index{ _find_target(subVec, false, msgId) };
            if ((index < subVec.size()) && targets.add(subVec.value_at(index)))
            {
                _remove_at(subVec, index);
            }
        }
        else if (isRequest || isResponse)
        {
            uint32_t i = 0;
            while (i < subVec.size())
            {
                const Listener & listener{ subVec.value_at(i) };
                if (listener.is_request() && (isResponse || (listener.mMessageId == msgId)) && targets.add(listener))
                {
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
            static_cast<void>(targets.append(subVec));
        }
    } while (false);

    for (uint32_t i = 0; i < targets.size(); ++i)
    {
        const StubBase::Listener & listener = targets.value_at(i);
        if ((listener.mFlags & Listener::FLAG_ORPHANED) != 0u)
            continue;

        ServiceResponseEvent eventError{ isResponse
                                       ? create_response(listener.proxy(), listener.mMessageId, areg::CallType::Request, areg::ResultType::RequestError, SharedBuffer{})
                                       : isBroadcast
                                       ? create_response(listener.proxy(), msgId, areg::CallType::Broadcast, areg::ResultType::DataInvalid, SharedBuffer{})
                                       : masterEvent.clone_for_target(listener.mConsumer, listener.mRawService) };
        if (eventError.is_valid())
        {
            eventError.set_sequence_number(listener.mSequenceNr);
            send_service_response(eventError);
        }
    }

    if (cancelAll)
    {
        _cancel_sessions(msgId, listSlot);
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
    mCurrSlot = INVALID_SLOT;
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

    constexpr areg::CallType kinds[]{ areg::CallType::Attribute, areg::CallType::Response, areg::CallType::Broadcast, areg::CallType::Request };
    for (const areg::CallType kind : kinds)
    {
        const uint32_t count{ mInterface.count(kind) };
        for (uint32_t i = 0; i < count; ++i)
        {
            error_request(kind, i, false);
        }
    }
}

void StubBase::cancel_all_requests()
{
    const uint32_t numOfReqs= number_of_requests();
    const uint32_t* reqIds  = request_ids();
    for ( uint32_t i = 0; i < numOfReqs; ++ i )
    {
        error_request( areg::CallType::Request, i, true );
        const uint32_t respIndex{ mInterface.response_index(i) };
        if (respIndex != areg::RESPONSE_INDEX_NONE)
        {
            _cancel_sessions(reqIds[i], _slot(areg::CallType::Response, respIndex));
        }
    }
}

void StubBase::invalidate_attribute( uint32_t attrIndex )
{
    if ( attrIndex < number_of_attributes() )
        error_request(areg::CallType::Attribute, attrIndex, false);
}

void StubBase::send_update_event( areg::CallType kind, uint32_t index, const SharedBuffer & data, areg::ResultType result ) const
{
    LOG_SCOPE( areg_component_StubBase, send_update_event );
    StubBase::StubListenerList listeners;
    if ((kind != areg::CallType::Request) && (find_listeners(kind, index, listeners) > 0))
    {
        const ProxyAddress proxy{ listeners.first_entry().proxy() };
        const uint32_t msgId{ mInterface.message_id(kind, index) };
        LOG_DBG( "Sending update of message [ %u ] with result [ %s ] to [ %u ] subscribed consumer(s), first is [ %s ]"
                   , msgId
                   , areg::as_string( result )
                   , listeners.size()
                   , ProxyAddress::to_path( proxy ).as_string() );

        ServiceResponseEvent eventElem = create_response(proxy, msgId, kind, result, data);
        if (eventElem.is_valid())
        {
            send_update_notification(listeners, eventElem);
        }
    }
}

void StubBase::send_notify_once( const ProxyAddress & target, uint32_t msgId, areg::CallType kind, const SharedBuffer & data, areg::ResultType result ) const
{
    ServiceResponseEvent eventElem = create_response( target, msgId, kind, result, data );
    if ( eventElem.is_valid() )
    {
        send_service_response( eventElem );
    }
}

void StubBase::send_response_event( uint32_t respIndex, const SharedBuffer & data )
{
    StubBase::StubListenerList listeners;
    collect_and_strip_response_listeners(areg::CallType::Response, respIndex, listeners);
    if (listeners.size() > 0)
    {
        ServiceResponseEvent eventElem = create_response(listeners.first_entry().proxy(), mInterface.message_id(areg::CallType::Response, respIndex), areg::CallType::Response, areg::ResultType::RequestOK, data);
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
    ServiceResponseEvent eventElem = create_response(proxy, whichListener.mMessageId, areg::CallType::Request, areg::ResultType::RequestBusy, SharedBuffer{});
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

bool StubBase::can_execute_request( Listener & whichListener, uint32_t respIndex, const SequenceNumber & seqNr )
{
    if ((seqNr == areg::SEQUENCE_NUMBER_NOTIFY) || (seqNr == areg::SEQUENCE_NUMBER_ANY))
    {
        LOG_SCOPE(areg_component_StubBase, can_execute_request);
        LOG_WARN("Request of response [ %u ] of service [ %s ] has the invalid sequence number [ %llu ], it is dropped"
                    , respIndex, mAddress.service_name().as_string(), static_cast<unsigned long long>(seqNr));
        return false;
    }

    bool result = false;
    if (is_busy(respIndex))
    {
        whichListener.mSequenceNr   = seqNr;
        send_busy_response(whichListener);
    }
    else
    {
        prepare_request(whichListener, seqNr, respIndex);
        result = true;
    }

    return result;
}

bool StubBase::exist( areg::CallType kind, uint32_t index, const ProxyAddress & notifySource ) const noexcept
{
    if (!notifySource.is_valid())
        return false;

    Lock lock(mListenerLock);
    const ListenerSlotList * subVec{ mListeners.at(_slot(kind, index)) };
    if (subVec == nullptr)
        return false;

    for (uint32_t i = 0; i < subVec->size(); ++i)
    {
        const StubBase::Listener & listener = subVec->value_at(i);
        if (listener.proxy_matches(notifySource) && (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr))
            return true;
    }

    return false;
}

bool StubBase::add_notification_listener( areg::CallType kind, uint32_t index, const ProxyAddress & notifySource )
{
    LOG_SCOPE( areg_component_StubBase, add_notification_listener );

    bool result { false };
    if (notifySource.is_valid())
    {
        Lock lock(mListenerLock);
        const uint32_t slot{ _slot(kind, index) };
        ListenerSlotList * subVec{ mListeners.at(slot) };
        if (subVec == nullptr)
            return false;

        const uint32_t msgId{ mInterface.message_id(kind, index) };
        bool hasEntry{ false };
        for (uint32_t i = 0; i < subVec->size(); ++i)
        {
            const StubBase::Listener & listener = subVec->value_at(i);
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

            StubBase::Listener listener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, notifySource);
            listener.mSlot = static_cast<uint16_t>(slot);
            subVec->add(listener);
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

void StubBase::remove_notification_listener( areg::CallType kind, uint32_t index, const ProxyAddress & notifySource ) noexcept
{
    Lock lock(mListenerLock);
    ListenerSlotList * found{ mListeners.at(_slot(kind, index)) };
    if (found == nullptr)
        return;

    ListenerSlotList & subVec = *found;
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
        MapProviderResource & providers{ map_providers() };
        Lock lock(providers.lockable());
        const bool registered{ providers.find_resource_object(static_cast<uint32_t>(mAddress)) == this };
        if (registered)
        {
            static_cast<void>(providers.unregister_resource_object(static_cast<uint32_t>(mAddress)));
        }

        mAddress = stubTarget;
        if (registered)
        {
            providers.register_resource_object(static_cast<uint32_t>(mAddress), this);
        }
    }

    mConnectionStatus = status;
}

const Version & StubBase::impl_version() const noexcept
{
    return mInterface.idVersion;
}

uint32_t StubBase::number_of_requests() const noexcept
{
    return mInterface.requests().mtCount;
}

uint32_t StubBase::number_of_responses() const noexcept
{
    return mInterface.responses().mtCount;
}

uint32_t StubBase::number_of_broadcasts() const noexcept
{
    return mInterface.broadcasts().mtCount;
}

uint32_t StubBase::number_of_attributes() const noexcept
{
    return mInterface.attributes().mtCount;
}

const uint32_t * StubBase::request_ids() const noexcept
{
    return mInterface.requests().mtIds;
}

const uint32_t * StubBase::response_ids() const noexcept
{
    return mInterface.responses().mtIds;
}

const uint32_t * StubBase::broadcast_ids() const noexcept
{
    return mInterface.broadcasts().mtIds;
}

const uint32_t * StubBase::attribute_ids() const noexcept
{
    return mInterface.attributes().mtIds;
}

ServiceResponseEvent StubBase::create_response( const ProxyAddress &    /* proxy  */
                                              , uint32_t                /* msgId  */
                                              , areg::CallType          /* kind   */
                                              , areg::ResultType        /* result */
                                              , const SharedBuffer &    /* data   */ ) const
{
    return ServiceResponseEvent(MessageEnvelope{});  // invalid; derived stubs override to produce real events
}

ServiceResponseEvent StubBase::create_response_event( const ProxyAddress &  /* proxy  */
                                                    , uint32_t              /* msgId  */
                                                    , areg::CallType        /* kind   */
                                                    , areg::ResultType      /* result */
                                                    , uint32_t              /* reserve */ ) const
{
    return ServiceResponseEvent(MessageEnvelope{});  // invalid; derived stubs override to produce real events
}

ServiceResponseEvent StubBase::create_undelivered_response( const ProxyAddress & proxy, uint32_t msgId, const SequenceNumber & seqNr ) const
{
    ServiceResponseEvent failure{ create_response(proxy, msgId, areg::CallType::Request, areg::ResultType::MessageUndelivered, SharedBuffer{}) };
    if (failure.is_valid())
    {
        failure.set_sequence_number(seqNr);
    }

    return failure;
}

ServiceResponseEvent StubBase::create_unsupported_response( const ProxyAddress & proxy, uint32_t msgId, const SequenceNumber & seqNr ) const
{
    ServiceResponseEvent failure{ create_response(proxy, msgId, areg::CallType::Request, areg::ResultType::RequestUnsupported, SharedBuffer{}) };
    if (failure.is_valid())
    {
        failure.set_sequence_number(seqNr);
    }

    return failure;
}

ServiceResponseEvent StubBase::prepare_response_event( areg::CallType kind, uint32_t index, areg::ResultType result, uint32_t reserve, StubBase::StubListenerList & listeners )
{
    collect_and_strip_response_listeners(kind, index, listeners);
    if (listeners.size() > 0)
        return create_response_event(listeners.first_entry().proxy(), mInterface.message_id(kind, index), kind, result, reserve);

    return ServiceResponseEvent(MessageEnvelope{});  // no listeners: invalid event
}

void StubBase::process_stub_event( StubEvent & /* eventElem */ )
{
}

void StubBase::process_generic_event(Event & /* eventElem */)
{
}

} // namespace areg
