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
#include "areg/component/EventDataStream.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/private/StubConnectEvent.hpp"

#include "areg/logging/areg_log.h"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// StubBase class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE( areg_component_StubBase_startupServiceInterface );
DEF_LOG_SCOPE( areg_component_StubBase_shutdownServiceIntrface );
DEF_LOG_SCOPE( areg_component_StubBase_errorAllRequests );
DEF_LOG_SCOPE( areg_component_StubBase_sendUpdateEvent);
DEF_LOG_SCOPE( areg_component_StubBase_sendBusyRespone );
DEF_LOG_SCOPE( areg_component_StubBase_clientConnected );
DEF_LOG_SCOPE( areg_component_StubBase_addNotificationListener );

//////////////////////////////////////////////////////////////////////////
// StubBase::Listener implementation
//////////////////////////////////////////////////////////////////////////

bool StubBase::Listener::operator == ( const StubBase::Listener & other ) const noexcept
{
    bool result = this == &other;
    if (result == false)
    {
        if (mMessageId == other.mMessageId)
        {
            if (areg::SEQUENCE_NUMBER_ANY == other.mSequenceNr)
            {
                result = true;
            }
            else if (mSequenceNr == other.mSequenceNr || mSequenceNr == static_cast<uint32_t>(-1 * static_cast<int32_t>(mSequenceNr == other.mSequenceNr)))
            {
                result = true;
            }
        }
    }

    return result;
}


//////////////////////////////////////////////////////////////////////////
// StubBase implementation
//////////////////////////////////////////////////////////////////////////

inline StubBase::MapStubResource& StubBase::map_providers() noexcept
{
    static StubBase::MapStubResource   _mapProviders;
    return _mapProviders;
}



StubBase::StubBase( Component & masterComp, const areg::InterfaceData & siData )
    : StubEventConsumer   ( mAddress )

    , mComponent            (masterComp)
    , mInterface            (siData)
    , mAddress              (siData, masterComp.address().role_name(), masterComp.address().thread_address().name())
    , mConnectionStatus     ( areg::ServiceConnectionState::Disconnected )
    , mListListener         ( )
    , mCurrListener         (mListListener.invalid_position())
    , mSessionId            (0)
    , mMapSessions          ( )
{
    map_providers().register_resource_object(mAddress, this);
    masterComp.register_service_provider(self());
}

StubBase::~StubBase()
{
    map_providers().unregister_resource_object(mAddress);
}

bool StubBase::is_busy( uint32_t requestId ) const noexcept
{
    bool result = false;
    StubBase::StubListenerList::LISTPOS pos = mListListener.find(StubBase::Listener(requestId, areg::SEQUENCE_NUMBER_ANY));
    for ( ; (result == false) && mListListener.is_valid_position(pos); pos = mListListener.next_position(pos))
    {
        result = mListListener.value_at(pos).mSequenceNr != 0;
    }

    return result;
}

SessionID StubBase::unblock_current_request()
{
    SessionID result = StubBase::INVALID_SESSION_ID;
    StubBase::Listener listener;
    if (mListListener.is_valid_position(mCurrListener) )
    {
        mListListener.remove_at(mCurrListener, listener);
        result = ++ mSessionId;
        mMapSessions.set_value_at(result, listener);
        mCurrListener   = mListListener.invalid_position();
    }

    return result;
}

bool StubBase::prepare_response( SessionID sessionId )
{
    bool result{ false };
    StubBase::Listener listener;
    if (mMapSessions.remove_at(sessionId, listener))
    {
        mListListener.push_first(listener);
        result = true;
    }

    return result;
}

void StubBase::prepare_request( Listener & listener, const SequenceNumber & seqNr, uint32_t responseId )
{
    listener.mMessageId = responseId;
    listener.mSequenceNr= mListListener.is_valid_position(mListListener.find(listener)) ? static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(seqNr)) : seqNr;
    mListListener.push_first(listener);
    mCurrListener = mListListener.first_position();
}

uint32_t StubBase::find_listeners( uint32_t requestId, StubListenerList & out_listners ) const
{
    StubBase::Listener listener(requestId, areg::SEQUENCE_NUMBER_ANY);
    StubListenerList::LISTPOS pos = mListListener.find(listener);
    while (mListListener.is_valid_position(pos))
    {
        out_listners.push_last(mListListener[pos]);
        pos = mListListener.find(listener, pos);
    }

    return out_listners.size();
}

void StubBase::clear_all_listeners( const ProxyAddress & whichProxy, IntegerArray & removedIDs )
{
    StubListenerList::LISTPOS pos = mListListener.first_position();
    while ( mListListener.is_valid_position(pos))
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            removedIDs.add(mListListener.value_at(pos).mMessageId);
            pos = mListListener.remove_at(pos);
        }
        else
        {
            pos = mListListener.next_position(pos);
        }
    }
}

void StubBase::clear_all_listeners( const ProxyAddress & whichProxy ) noexcept
{
    StubListenerList::LISTPOS pos = mListListener.first_position();
    while ( mListListener.is_valid_position(pos) )
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            pos = mListListener.remove_at(pos);
        }
        else
        {
            pos = mListListener.next_position(pos);
        }
    }
}

void StubBase::send_response_notification( const StubListenerList & whichListeners, const ServiceResponseEvent& masterEvent )
{
    for(StubListenerList::LISTPOS pos = whichListeners.first_position(); whichListeners.is_valid_position(pos); pos = whichListeners.next_position(pos) )
    {
        const StubBase::Listener& listener = whichListeners[pos];
        ServiceResponseEvent* eventResp = masterEvent.clone_for_target(listener.mProxy);
        if (eventResp != nullptr)
        {
            if (static_cast<int32_t>(listener.mSequenceNr) >= 0)
            {
                eventResp->set_sequence_number(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    mListListener.remove_entry(listener);
            }
            else
            {
                eventResp->set_sequence_number(static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(listener.mSequenceNr)));
                StubBase::Listener removed(masterEvent.response_id(), 0, listener.mProxy);
                mListListener.remove_entry(removed);
            }

            send_service_response(*eventResp);
        }
    }
}

void StubBase::send_error_notification( const StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent )
{
    for(StubListenerList::LISTPOS pos = whichListeners.first_position(); whichListeners.is_valid_position(pos); pos = whichListeners.next_position(pos))
    {
        const StubBase::Listener& listener = whichListeners[pos];
        ServiceResponseEvent* eventError = masterEvent.clone_for_target(listener.mProxy);
        if (eventError != nullptr)
        {
            if (static_cast<int32_t>(listener.mSequenceNr) >= 0)
            {
                eventError->set_sequence_number(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    mListListener.remove_entry(listener);
            }
            else
            {
                eventError->set_sequence_number(static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(listener.mSequenceNr)));
            }

            send_service_response(*eventError);
        }
    }
}

void StubBase::send_update_notification( const StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent ) const
{
    for (StubListenerList::LISTPOS pos = whichListeners.first_position(); whichListeners.is_valid_position(pos); pos = whichListeners.next_position(pos))
    {
        const StubBase::Listener& listener = whichListeners[pos];
        ServiceResponseEvent* eventResp = masterEvent.clone_for_target(listener.mProxy);
        if ( eventResp != nullptr )
        {
            send_service_response( *eventResp );
        }
    }
}

void StubBase::send_service_response( ServiceResponseEvent & eventElem ) const
{
    eventElem.target_proxy().deliver_service_event(eventElem);
}

void StubBase::cancel_current_request() noexcept
{
    mCurrListener   = mListListener.invalid_position();
}

ComponentThread & StubBase::component_thread() const noexcept
{
    return mComponent.master_thread();
}

StubBase* StubBase::find_stub( const StubAddress& address ) noexcept
{
    return map_providers().find_resource_object(address);
}

void StubBase::startup_service_interface( Component&  holder )
{
    LOG_SCOPE( areg_component_StubBase_startupServiceInterface );
    LOG_DBG( "Service with role [ %s ] and interface [ %s ] is started", service_role( ).as_string( ), service_name( ).as_string( ) );

    StubConnectEvent::add_listener( static_cast<StubEventConsumer &>(self()), holder.master_thread() );
}

void StubBase::shutdown_service_interface( Component & holder ) noexcept
{
    LOG_SCOPE( areg_component_StubBase_shutdownServiceIntrface );
    LOG_INFO( "Service with role [ %s ] and interface [ %s ] is stopped", service_role().as_string(), service_name().as_string() );
    StubConnectEvent::remove_listener( static_cast<StubEventConsumer &>(self()), holder.master_thread() );
}

void StubBase::error_all_requests()
{
    LOG_SCOPE( areg_component_StubBase_errorAllRequests );
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

void StubBase::send_update_event( uint32_t msgId, const EventDataStream & data, areg::ResultType result ) const
{
    LOG_SCOPE( areg_component_StubBase_sendUpdateEvent);
    StubBase::StubListenerList listeners;
    if (find_listeners(msgId, listeners) > 0)
    {
        const ProxyAddress & proxy = listeners.first_entry( ).mProxy;
        LOG_WARN( "Sends busy message to proxy [ %s ] for the request [ %u ]", ProxyAddress::to_path( proxy).as_string(), msgId);

        ResponseEvent* eventElem = create_response(proxy, msgId, result, data);
        if (eventElem != nullptr)
        {
            send_update_notification(listeners, *eventElem);
            eventElem->destroy();
        }
    }
}

void StubBase::send_notify_once( const ProxyAddress & target, uint32_t msgId, const EventDataStream & data, areg::ResultType result ) const
{
    ResponseEvent * eventElem = create_response( target, msgId, result, data );
    if ( eventElem != nullptr )
    {
        send_service_response( *eventElem );
    }
}

void StubBase::send_response_event( uint32_t respId, const EventDataStream & data )
{
    StubBase::StubListenerList listeners;
    if (find_listeners(respId, listeners) > 0)
    {
        ResponseEvent* eventElem = create_response(listeners.first_entry().mProxy, respId, areg::ResultType::RequestOK, data);
        if (eventElem != nullptr)
        {
            send_response_notification(listeners, *eventElem);
            eventElem->destroy();
        }
    }
}

void StubBase::send_busy_respone( const Listener & whichListener )
{
    LOG_SCOPE(areg_component_StubBase_sendBusyRespone);
    ResponseEvent* eventElem = create_response(whichListener.mProxy, whichListener.mMessageId, areg::ResultType::RequestBusy, EventDataStream::empty_data());
    if (eventElem != nullptr)
    {
        LOG_WARN("Sending busy response for request message [ %p ] from source [ %p ] to target [ %p ], sequence [ %llu ]"
                    , whichListener.mMessageId
                    , whichListener.mProxy.target()
                    , whichListener.mProxy.source()
                    , whichListener.mSequenceNr);

        eventElem->set_sequence_number(whichListener.mSequenceNr);
        send_service_response(*eventElem);
    }
}

bool StubBase::can_execute_request( Listener & whichListener, uint32_t whichResponse, const SequenceNumber & seqNr )
{
    bool result = false;
    if (is_busy(whichResponse))
    {
        whichListener.mSequenceNr   = seqNr;
        send_busy_respone(whichListener);
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
    bool result = false;
    if ( notifySource.is_valid() )
    {
        StubListenerList::LISTPOS pos = mListListener.first_position();
        for ( ; (result == false) && mListListener.is_valid_position(pos); pos = mListListener.next_position(pos) )
        {
            const StubBase::Listener & listener = mListListener.value_at(pos);
            result = (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                     (msgId == listener.mMessageId) &&
                     (notifySource == listener.mProxy);
        }
    }

    return result;
}

bool StubBase::add_notification_listener(uint32_t msgId, const ProxyAddress & notifySource)
{
    LOG_SCOPE(areg_component_StubBase_addNotificationListener);

    bool result { false };
    if (notifySource.is_valid())
    {
        bool hasEntry{ false };
        auto pos = mListListener.first_position();
        for ( ; (hasEntry == false) && mListListener.is_valid_position(pos); pos = mListListener.next_position(pos) )
        {
            const StubBase::Listener & listener = mListListener.value_at(pos);
            hasEntry = (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                       (msgId == listener.mMessageId) &&
                       (notifySource == listener.mProxy);
        }

        if ( hasEntry == false)
        {
            LOG_DBG("For the message [ %u ] new listener [ %s ] is added"
                        , msgId
                        , ProxyAddress::to_path(notifySource).as_string());

            mListListener.push_last(StubBase::Listener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, notifySource));
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
    for (StubListenerList::LISTPOS pos = mListListener.first_position(); mListListener.is_valid_position(pos); pos = mListListener.next_position(pos) )
    {
        const StubBase::Listener & listener = mListListener.value_at(pos);
        if ( areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && msgId == listener.mMessageId && notifySource == listener.mProxy )
        {
            mListListener.remove_at(pos);
            break;
        }
    }
}

bool StubBase::client_connected(const ProxyAddress & client, areg::ServiceConnectionState status )
{
    bool result{ false };
    if (mAddress == client)
    {
        LOG_SCOPE(areg_component_StubBase_clientConnected);
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
    client_connected( proxyAddress, status );
}

void StubBase::process_registered_event(const StubAddress & stubTarget, areg::ServiceConnectionState status )
{
    if ( areg::is_service_connected( status) )
    {
        ASSERT( stubTarget.is_valid() );
        map_providers().lock();
        map_providers().unregister_resource_object(mAddress);

        mAddress = stubTarget;
        
        map_providers().register_resource_object(mAddress, this);
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

ResponseEvent * StubBase::create_response( const ProxyAddress &     /* proxy */
                                         , uint32_t             /* msgId */
                                         , areg::ResultType   /* result */
                                         , const EventDataStream &  /* data */ ) const
{
    return nullptr;
}

RemoteRequestEvent * StubBase::create_remote_request( const InStream & /* stream */ ) const
{
    return nullptr;
}

RemoteNotifyRequestEvent * StubBase::create_notify_request( const InStream & /* stream */ ) const
{
    return nullptr;
}

void StubBase::process_stub_event( StubEvent & /* eventElem */ )
{
}

void StubBase::process_generic_event(Event & /* eventElem */)
{
}

} // namespace areg
