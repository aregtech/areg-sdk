/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Base class implementation.
 *
 ************************************************************************/
#include "areg/component/ProxyBase.hpp"

#include "areg/base/Version.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/ProxyListener.hpp"

#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/ComponentInfo.hpp"
#include "areg/component/private/ServiceManager.hpp"

#include "areg/logging/areg_log.h"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Predefined constants / statics
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_ProxyBase, acquire_proxy);
DEF_LOG_SCOPE(areg_component_ProxyBase, request_failure_event);
DEF_LOG_SCOPE(areg_component_ProxyBase, service_connection_updated);
DEF_LOG_SCOPE(areg_component_ProxyBase, unregister_listener);
DEF_LOG_SCOPE(areg_component_ProxyBase, prepare_listeners);
DEF_LOG_SCOPE(areg_component_ProxyBase, process_available_event);
DEF_LOG_SCOPE(areg_component_ProxyBase, stop_proxy);

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ImplProxyMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase::ServiceAvailableEvent::ServiceAvailableEvent( NotificationConsumer & consumer )
    : Event ( areg::EventType::EventCustomExternal )
{
    set_event_consumer(&consumer);
    set_call_type( 0u );
}


//////////////////////////////////////////////////////////////////////////
// ProxyBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, static methods
//////////////////////////////////////////////////////////////////////////

ProxyBase::MapProxyResource& ProxyBase::map_proxies() noexcept
{
    static MapProxyResource _mapProxies;
    return _mapProxies;
}

ProxyBase::MapThreadProxyList& ProxyBase::thread_proxies() noexcept
{
    static MapThreadProxyList   _threadProxies;
    return _threadProxies;
}

std::shared_ptr<ProxyBase> ProxyBase::acquire_proxy( const String & roleName
                                                   , const areg::InterfaceData & serviceIfData
                                                   , ProxyListener & connect
                                                   , FuncCreateProxy funcCreate
                                                   , const String & ownerThread /*= String::empty_string()*/)
{
    return ProxyBase::acquire_proxy(roleName, serviceIfData, connect, funcCreate, DispatcherThread::dispatcher_thread(ThreadAddress(ownerThread)) );
}

std::shared_ptr<ProxyBase> ProxyBase::acquire_proxy(const String& roleName, const areg::InterfaceData& serviceIfData, ProxyListener& connect, FuncCreateProxy funcCreate, const UniqueNumber ownerThread)
{
    return ProxyBase::acquire_proxy(roleName, serviceIfData, connect, funcCreate, DispatcherThread::dispatcher_thread(ownerThread));
}

std::shared_ptr<ProxyBase> ProxyBase::acquire_proxy( const String & roleName
                                                   , const areg::InterfaceData & serviceIfData
                                                   , ProxyListener & connect
                                                   , FuncCreateProxy funcCreate
                                                   , DispatcherThread & ownerThread )
{
    LOG_SCOPE( areg_component_ProxyBase, acquire_proxy );

    std::shared_ptr<ProxyBase> proxy{ nullptr };
    if (!ownerThread.is_valid())
        return proxy;

    MapProxyResource& mapProxies{ map_proxies() };
    ProxyAddress Key(serviceIfData, roleName, ownerThread.name() );
    proxy = mapProxies.find_resource_object(static_cast<uint32_t>(Key));
    if (proxy.get() == nullptr)
    {
        LOG_DBG("No proxy [ %s ] found, creating one in thread [ %u ]", ProxyAddress::to_path(Key).as_string(), ownerThread.id());
        std::shared_ptr<ProxyBase> newProxy{ funcCreate( roleName, &ownerThread ) };
        if ( newProxy.get() != nullptr )
        {
            proxy.swap( newProxy );
            mapProxies.register_resource_object( static_cast<uint32_t>(proxy->mProxyAddress), proxy );
            thread_proxies().register_resource_object( static_cast<uint32_t>(proxy->mDispatcherThread.address()), proxy);
        }
    }

    if (proxy.get() == nullptr)
    {
        return proxy;
    }
    
    if (!proxy->mListConnect.add_if_unique(&connect))
    {
        LOG_WARN("The client [ %p ] is already registered for service connection notification", &connect);
        return proxy;
    }

    LOG_DBG("Add Service Connect notification for client [ %p ]", &connect);
    static_cast<void>(proxy->add_listener( CONNECTION_ID, areg::SEQUENCE_NUMBER_NOTIFY, static_cast<NotificationConsumer *>(&connect), true ));
    ++ proxy->mProxyInstCount;
    proxy->mIsStopped = false;
    if ( proxy->mProxyInstCount == 1 )
    {
        proxy->register_service_listeners( );
        // Deferred during startup: run_dispatcher() registers all proxies after start_components() completes.
        if (!ownerThread.startup_phase())
        {
            ServiceManager::request_register_consumer( proxy->proxy_address( ) );
        }
    }
    else if ( proxy->is_connected() )
    {
        ProxyBase::ServiceAvailableEvent event{ proxy->create_service_available(connect) };
        proxy->send_service_event( event );
    }

    return proxy;
}

int32_t ProxyBase::find_thread_proxies(DispatcherThread & ownerThread, ArrayList<std::shared_ptr<ProxyBase>> & threadProxyList )
{
    ThreadProxyList * proxyList = ProxyBase::thread_proxies().find_resource(static_cast<uint32_t>(ownerThread.address()));
    int32_t result = proxyList != nullptr ? static_cast<int32_t>(proxyList->size()) : 0;
    if ( result > 0 )
    {
        threadProxyList = static_cast<const ArrayList<std::shared_ptr<ProxyBase>> &>(*proxyList);
    }

    return result;
}

ServiceResponseEvent ProxyBase::request_failure_event(const ProxyAddress & target, uint32_t msgId, areg::ResultType errCode, const SequenceNumber & seqNr)
{
    LOG_SCOPE( areg_component_ProxyBase, request_failure_event);

    std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy(static_cast<uint32_t>(target));
    return (proxy.get() != nullptr ? proxy->create_request_failed(target, msgId, errCode, seqNr) : ResponseEvent(MessageEnvelope{}));
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyBase::ProxyBase(const String & roleName, const areg::InterfaceData & serviceIfData, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyEventConsumer  ( mProxyAddress )

    , mProxyAddress     ( serviceIfData, roleName, (ownerThread != nullptr) && (ownerThread->is_valid()) ? ownerThread->name() : String::empty_string() )
    , mStubAddress      ( StubAddress::invalid_stub_address() )
    , mSequenceCount    ( 0 )
    , mListenerMap      ( )
    , mListenerLock     ( )
    , mListConnect      (   )
    , mProxyInstCount   ( 0 )

    , mIsStopped        ( false )

    , mProxyData        ( serviceIfData )

    , mDispatcherThread ( (ownerThread != nullptr) && (ownerThread->is_valid()) ? *ownerThread : DispatcherThread::dispatcher_thread( mProxyAddress.thread()) )
    , mConnectionStatus ( areg::ServiceConnectionState::Unknown )
    , mIsConnected      ( false )
{
    ASSERT(mDispatcherThread.is_valid());
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, methods
//////////////////////////////////////////////////////////////////////////

bool ProxyBase::has_any_listener( uint32_t msgId ) const noexcept
{
    Lock lock(mListenerLock);
    ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    return mListenerMap.is_valid_position(pos) && (pos->second.size() != 0u);
}

bool ProxyBase::has_notification_listener( uint32_t msgId ) const noexcept
{
    Lock lock(mListenerLock);
    ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
        return false;

    const ProxyListenerList & subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if (subVec.value_at(i).mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY)
            return true;
    }
    return false;
}

bool ProxyBase::add_listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer * caller, bool unique )
{
    Lock lock(mListenerLock);
    ProxyBase::Listener listener{ seqNr, caller };
    ProxyListenerList & subVec = mListenerMap[msgId];
    if (unique)
        return subVec.add_if_unique(listener);

    subVec.add(listener);
    return true;
}

void ProxyBase::remove_listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer * caller ) noexcept
{
    Lock lock(mListenerLock);
    ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (mListenerMap.is_valid_position(pos))
        static_cast<void>(pos->second.remove_elem(ProxyBase::Listener{ seqNr, caller }));
}

void ProxyBase::register_service_listeners()
{
    ProxyConnectEvent::add_listener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::unregister_service_listeners() noexcept
{
    ProxyConnectEvent::remove_listener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::free_proxy( ProxyListener & connect )
{
    int32_t exists = mListConnect.find(&connect, 0);
    if ( exists >= 0 )
    {
        mListConnect.remove_at(static_cast<uint32_t>(exists));
        connect.service_connected(areg::ServiceConnectionState::Disconnected, self());
    }

    remove_listener( static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                   , areg::SEQUENCE_NUMBER_NOTIFY
                   , static_cast<NotificationConsumer *>(&connect));

    std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy( mProxyAddress );

    if ((proxy.use_count() == 1) || (mProxyInstCount == 1))
    {
        if ( !mIsStopped )
        {
            stop_all_notifications( );
            unregister_service_listeners( );
            {
                Lock lock(mListenerLock);
                mListenerMap.clear();
            }

            ServiceManager::request_unregister_consumer( proxy_address( ), areg::DisconnectReason::ConsumerDisconnected );
            mDispatcherThread.remove_consumer( *this );
        }

        set_connection_status( areg::ServiceConnectionState::Disconnected );
        mIsStopped   = true;

        mProxyInstCount = 0;
        map_proxies().unregister_resource_object(static_cast<uint32_t>(mProxyAddress));
        thread_proxies().unregister_resource_object( static_cast<uint32_t>(mDispatcherThread.address( )), proxy, true );
    }
    else if ( mProxyInstCount > 0 )
    {
        -- mProxyInstCount;
    }
}

void ProxyBase::terminate_self()
{
    if (mProxyInstCount == 0)
        return;

    {
        Lock lock(mListenerLock);
        mListenerMap.clear();
    }
    if (!mIsStopped)
    {
        ServiceManager::request_unregister_consumer(proxy_address(), areg::DisconnectReason::ConsumerDisconnected );
    }

    set_connection_status( areg::ServiceConnectionState::Disconnected );
    mIsStopped      = true;
    mProxyInstCount = 0;

    delete this;
}

void ProxyBase::service_connection_updated( const StubAddress & server, const Channel & channel, areg::ServiceConnectionState status )
{
    LOG_SCOPE( areg_component_ProxyBase, service_connection_updated );

    ASSERT(server.is_valid());
    if (!server.is_proxy_compatible(proxy_address()))
        return;

    LOG_DBG("The proxy [ %s ] have got [ %s ] status update notification event from stub [ %s ]"
                , ProxyAddress::to_path(proxy_address()).as_string()
                , areg::as_string(status)
                , StubAddress::to_path(server).as_string());
 
    // Local: channel.target = stub's thread CRC32 = server.source(). Must match when connected.
    ASSERT(channel.target() == server.source() || !server.is_local_address() || status != areg::ServiceConnectionState::Connected);
    mProxyAddress.set_channel(channel);
    set_connection_status( status );
    bool proxyConnected{ is_connected() };
    if ( proxyConnected )
    {
        mStubAddress = server;
    }
    else
    {
        mStubAddress = StubAddress::invalid_stub_address();
        mProxyData.reset();
    }

    ProxyListenerList connectListeners;
    {
        Lock lock(mListenerLock);
        ProxyListenerMap::MAPPOS mapPos = mListenerMap.find(CONNECTION_ID);
        if (mListenerMap.is_valid_position(mapPos))
            connectListeners = mapPos->second;
    }

    for (uint32_t i = 0u; i < connectListeners.size(); ++i)
    {
        ProxyListener* connect = static_cast<ProxyListener*>(connectListeners[i].mListener);

        if (proxyConnected)
        {
            mListConnect.add_if_unique(connect);
        }
        else
        {
            mListConnect.remove_elem(connect, 0);
        }

        connect->service_connected(status, *this);
    }
}

void ProxyBase::set_notification( uint32_t msgId, NotificationConsumer* caller, bool alwaysNotify /*= false*/ )
{
    if (!is_connected())
        return;

    bool hasListener;
    bool added;
    {
        Lock lock(mListenerLock);
        hasListener = has_notification_listener(msgId);   // recursive lock: safe
        added = add_listener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, caller, hasListener);   // recursive lock: safe
    }

    if ( added )
    {
        // new listener, if attribute, send actual data.
        if (areg::is_attribute_id(msgId))
        {
            send_notification_event( msgId
                                   , mProxyData.attribute_state(msgId) == areg::DataState::DataIsOK ? areg::ResultType::DataOK : areg::ResultType::DataInvalid
                                   , areg::SEQUENCE_NUMBER_NOTIFY, caller);
        }

        // assign only if there was no listener
        if (hasListener == false)
        {
            start_notification(msgId);
        }
    }
    else if ( alwaysNotify )
    {
        // the listener already exists
        if ( areg::is_attribute_id(msgId) )
        {
            send_notification_event( msgId
                                   , mProxyData.attribute_state(msgId) == areg::DataState::DataIsOK ? areg::ResultType::DataOK : areg::ResultType::DataInvalid
                                   , areg::SEQUENCE_NUMBER_NOTIFY, caller);
        }
        else if ( areg::is_response_id(msgId) && (mProxyData.param_state(msgId) == areg::DataState::DataIsOK) )
        {
            send_notification_event(  msgId, areg::ResultType::RequestOK, areg::SEQUENCE_NUMBER_NOTIFY, caller);
        }
    }
}

void ProxyBase::clear_notification( uint32_t msgId, NotificationConsumer* caller )
{
    bool noListeners;
    {
        Lock lock(mListenerLock);
        remove_listener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, caller);   // recursive lock: safe
        noListeners = !has_notification_listener(msgId);                // recursive lock: safe
    }
    if (noListeners)
    {
        stop_notification(msgId);
        mProxyData.set_data_state(msgId, areg::DataState::DataIsUnavailable);
    }
}

void ProxyBase::unregister_listener( NotificationConsumer *consumer )
{
    LOG_SCOPE( areg_component_ProxyBase, unregister_listener );
    LOG_DBG("Unregisters proxy client [ %p ]", consumer);

    ArrayList<uint32_t> stopIds;
    {
        Lock lock(mListenerLock);
        for (ProxyListenerMap::MAPPOS mapPos = mListenerMap.first_position(); mListenerMap.is_valid_position(mapPos); )
        {
            ProxyListenerList & subVec = mapPos->second;
            const uint32_t msgId = mapPos->first;
            bool removed { false };
            uint32_t i = 0;
            while (i < subVec.size())
            {
                if (subVec.value_at(i).mListener != consumer)
                {
                    ++i;
                    continue;
                }

                LOG_DBG("Removes proxy client listener of message [ %u ] at index [ %d ]", msgId, i);
                removed = true;
                const uint32_t last = subVec.size() - 1u;
                if (i != last)
                    subVec.set_value_at(i, subVec.value_at(last));
                subVec.remove_at(last);
            }

            if (removed && !has_notification_listener(msgId))  // recursive lock: safe
                stopIds.add(msgId);

            mapPos = subVec.is_empty() ? mListenerMap.remove_at(mapPos) : mListenerMap.next_position(mapPos);
        }
    }

    for (uint32_t i = 0; i < stopIds.size(); ++i)
    {
        stop_notification(stopIds.value_at(i));
        mProxyData.set_data_state(stopIds.value_at(i), areg::DataState::DataIsUnavailable);
    }
}

uint32_t ProxyBase::prepare_listeners( ProxyBase::ProxyListenerList& out_listenerList, uint32_t msgId, const SequenceNumber & seqNrToSearch )
{
    LOG_SCOPE( areg_component_ProxyBase, prepare_listeners );

    Lock lock(mListenerLock);
    ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
    {
        LOG_DBG("Prepared [ 0 ] proxy clients for message [ %u ] and sequence number [ %llu ]", msgId, seqNrToSearch);
        return 0;
    }

    ProxyListenerList & subVec = pos->second;
    out_listenerList.reserve(subVec.size());

    for (uint32_t i = 0; i < subVec.size(); )
    {
        const ProxyBase::Listener & elem = subVec.value_at(i);
        if (elem.mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY)
        {
            out_listenerList.add(elem);
            ++i;
        }
        else if (elem.mSequenceNr == seqNrToSearch)
        {
            out_listenerList.add(elem);
            subVec.remove_at(i);
        }
        else
        {
            ++i;
        }
    }

    LOG_DBG("Prepared [ %d ] proxy clients for message [ %u ] and sequence number [ %llu ]", out_listenerList.size(), msgId, seqNrToSearch);
    return out_listenerList.size();
}

void ProxyBase::notify_listeners( uint32_t respId, areg::ResultType result, const SequenceNumber & seqNrToSearch )
{
    ProxyBase::ProxyListenerList listenerList;
    prepare_listeners(listenerList, respId, seqNrToSearch);
    for (uint32_t i = 0; i < listenerList.size(); ++ i)
    {
        const ProxyBase::Listener & elem = listenerList.value_at(i);
        elem.mListener->process_notification(respId, result, seqNrToSearch);
    }
}

void ProxyBase::send_notification_event( uint32_t msgId, areg::ResultType resType, const SequenceNumber & seqNr, NotificationConsumer* caller )
{
    NotificationEventData data(self(), resType, msgId, seqNr);
    NotificationEvent eventElem{ create_client_notification(data) };
    eventElem.set_event_consumer(static_cast<EventConsumer*>(caller));
    if (!mDispatcherThread.event_dispatcher().post_event(eventElem))
        eventElem.destroy_event();
}

void ProxyBase::process_proxy_event( [[maybe_unused]] ProxyEvent& eventElem )
{
    ASSERT(eventElem.target_proxy() == static_cast<uint32_t>(proxy_address()));
}

void ProxyBase::process_generic_event( Event& eventElem )
{
    const areg::EventHeader* hdr{ eventElem.header() };
    if (hdr != nullptr)
    {
        NotificationConsumer* consumer{ static_cast<NotificationConsumer*>(eventElem.event_consumer()) };
        const uint32_t delay{ static_cast<uint32_t>(hdr->callType) * 10u };
        if (consumer != nullptr)
            process_available_event(*consumer, delay);
    }
}

void ProxyBase::send_request_event(ServiceRequestEvent& reqEvent, NotificationConsumer* caller)
{
    if (!reqEvent.is_valid())
        return;

    uint32_t respId = proxy_data().response_id(reqEvent.message_id());
    ASSERT(areg::is_response_id(respId) || (respId == areg::RESPONSE_ID_NONE));
    if (respId != areg::RESPONSE_ID_NONE)
    {
        Lock lock(mListenerLock);
        ++mSequenceCount;
        static_cast<void>(add_listener(respId, mSequenceCount, caller, true));  // recursive lock: safe
        reqEvent.set_sequence_number(mSequenceCount);
    }

    mProxyAddress.deliver_service_event(reqEvent);
}

void ProxyBase::send_notify_request( uint32_t msgId, areg::RequestType reqType )
{
    ServiceRequestEvent notifyEvent{ create_notification_request(msgId, reqType) };
    send_notify_request(notifyEvent);
}

void ProxyBase::send_notify_request(ServiceRequestEvent& notifyEvent)
{
    if (notifyEvent.is_valid())
    {
        mProxyAddress.deliver_service_event(notifyEvent);
    }
}

void ProxyBase::send_service_event( ProxyBase::ServiceAvailableEvent& eventInstance )
{
    if ( eventInstance.is_valid() )
    {
        if (eventInstance.event_id() != static_cast<uint32_t>(RuntimeClassID::empty_id()))
        {
            eventInstance.add_listener(eventInstance.event_id(), self(), mDispatcherThread);
        }

        eventInstance.set_event_consumer(this);
        if (!eventInstance.register_for_thread(&mDispatcherThread) || !mDispatcherThread.event_dispatcher().post_event(eventInstance))
            eventInstance.destroy_event();
    }
}

bool ProxyBase::is_listener_registered( NotificationConsumer & caller ) const
{
    Lock lock(mListenerLock);
    ProxyListenerMap::MAPPOS mapPos = mListenerMap.find(CONNECTION_ID);
    if (mListenerMap.is_valid_position(mapPos) && !mapPos->second.is_empty())
    {
        const ProxyListenerList& subVec = mapPos->second;
        for (uint32_t i = 0; i < subVec.size(); ++i)
        {
            if (&caller == subVec.value_at(i).mListener)
                return true;
        }
    }

    return false;
}

void ProxyBase::process_available_event( NotificationConsumer & consumer, uint32_t delayEvent)
{
    LOG_SCOPE(areg_component_ProxyBase, process_available_event);
    if (is_connected() && is_listener_registered( consumer ) )
    {
        LOG_DBG("Notifying client [ %p ] the service connection status [ %s ]", &consumer, areg::as_string(connection_status()));
        if (delayEvent != areg::DO_NOT_WAIT)
        {
            Thread::sleep(delayEvent);
        }

        static_cast<ProxyListener&>(consumer).service_connected(connection_status(), self());
    }
}

void ProxyBase::stop_proxy()
{
    LOG_SCOPE( areg_component_ProxyBase, stop_proxy );

    if (mIsStopped)
        return;
    
    LOG_WARN("Going to stop proxy [ %s ]", ProxyAddress::to_path(mProxyAddress).as_string());

    for (uint32_t i = 0 ; i < mListConnect.size(); ++ i)
    {
        ProxyListener * listener = mListConnect.value_at(i);
        ASSERT(listener != nullptr);
        listener->service_connected( areg::ServiceConnectionState::Disconnected, *this);
    }

    mListConnect.clear();

    set_connection_status( areg::ServiceConnectionState::Disconnected );
    mIsStopped = true;

    stop_all_notifications( );
    unregister_service_listeners( );
    {
        Lock lock(mListenerLock);
        mListenerMap.clear();
    }
    ServiceManager::request_unregister_consumer( proxy_address( ), areg::DisconnectReason::ConsumerDisconnected );
    mDispatcherThread.remove_consumer( *this );

    mStubAddress = StubAddress::invalid_stub_address();
    mProxyData.reset();
    // mProxyAddress.set_channel(Channel::invalid_channel());
}

} // namespace areg
