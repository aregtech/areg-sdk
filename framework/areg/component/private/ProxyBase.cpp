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
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/ProxyListener.hpp"

#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/ComponentInfo.hpp"
#include "areg/component/private/ServiceManager.hpp"

#include "areg/logging/GELog.h"

//////////////////////////////////////////////////////////////////////////
// Predefined constants / statics
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_ProxyBase_findOrCreateProxy);
DEF_LOG_SCOPE(areg_component_ProxyBase_createRequestFailureEvent);
DEF_LOG_SCOPE(areg_component_ProxyBase_serviceConnectionUpdated);
DEF_LOG_SCOPE(areg_component_ProxyBase_unregisterListener);
DEF_LOG_SCOPE(areg_component_ProxyBase_prepareListeners);
DEF_LOG_SCOPE(areg_component_ProxyBase_stopProxy);

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyBase::Listener::Listener()
    : mMessageId    (static_cast<uint32_t>(NEService::FuncIdRange::EmptyFunctionId))
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( const Listener & src )
    : mMessageId    (src.mMessageId)
    , mSequenceNr   (src.mSequenceNr)
    , mListener     (src.mListener)
{
}

ProxyBase::Listener::Listener( Listener && src ) noexcept
    : mMessageId    ( src.mMessageId )
    , mSequenceNr   ( src.mSequenceNr )
    , mListener     ( src.mListener )
{
}

ProxyBase::Listener::Listener( uint32_t msgId )
    : mMessageId    (msgId)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( uint32_t msgId, const SequenceNumber & seqNr )
    : mMessageId    (msgId)
    , mSequenceNr   (seqNr)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer* caller )
    : mMessageId    (msgId)
    , mSequenceNr   (seqNr)
    , mListener     (caller)
{
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class, operators
//////////////////////////////////////////////////////////////////////////

ProxyBase::Listener & ProxyBase::Listener::operator = ( const ProxyBase::Listener & src )
{
    mMessageId  = src.mMessageId;
    mSequenceNr = src.mSequenceNr;
    mListener   = src.mListener;

    return (*this);
}

ProxyBase::Listener & ProxyBase::Listener::operator = ( ProxyBase::Listener && src ) noexcept
{
    mMessageId  = src.mMessageId;
    mSequenceNr = src.mSequenceNr;
    mListener   = src.mListener;

    return (*this);
}

bool ProxyBase::Listener::operator == ( const ProxyBase::Listener& other ) const
{
    bool result = this == &other ? true : false;
    if ((result == false) && (other.mMessageId == mMessageId))
    {
        if (other.mSequenceNr == NEService::SEQUENCE_NUMBER_ANY)
            result = true;
        else if (other.mSequenceNr == mSequenceNr && other.mListener == mListener)
            result = true;
        else if (other.mSequenceNr == NEService::SEQUENCE_NUMBER_NOTIFY && other.mListener == nullptr)
            result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ImplProxyMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

AREG_IMPLEMENT_RUNTIME_EVENT(ProxyBase::ServiceAvailableEvent, Event)

ProxyBase::ServiceAvailableEvent::ServiceAvailableEvent( NotificationConsumer & consumer )
    : Event             ( Event::EventType::EventExternal )
    , mNotifyConsumer   ( consumer )
    , mDelayConnectEvent( 0 )
{
}


//////////////////////////////////////////////////////////////////////////
// ProxyBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, static methods
//////////////////////////////////////////////////////////////////////////

ProxyBase::MapProxyResource& ProxyBase::map_proxies()
{
    static MapProxyResource _mapProxies;
    return _mapProxies;
}

ProxyBase::MapThreadProxyList& ProxyBase::thread_proxies()
{
    static MapThreadProxyList   _threadProxies;
    return _threadProxies;
}


std::shared_ptr<ProxyBase> ProxyBase::acquire_proxy( const String & roleName
                                                       , const NEService::InterfaceData & serviceIfData
                                                       , ProxyListener & connect
                                                       , FuncCreateProxy funcCreate
                                                       , const String & ownerThread /*= String::empty_string()*/)
{
    return ProxyBase::acquire_proxy(roleName, serviceIfData, connect, funcCreate, DispatcherThread::dispatcher_thread(ownerThread) );
}

std::shared_ptr<ProxyBase> ProxyBase::acquire_proxy( const String & roleName
                                                       , const NEService::InterfaceData & serviceIfData
                                                       , ProxyListener & connect
                                                       , FuncCreateProxy funcCreate
                                                       , DispatcherThread & ownerThread )
{
    LOG_SCOPE(areg_component_ProxyBase_findOrCreateProxy);

    std::shared_ptr<ProxyBase> proxy{ nullptr };
    if (ownerThread.is_valid())
    {
        ProxyAddress Key(serviceIfData, roleName, ownerThread.name() );
        proxy = map_proxies().find_resource_object(Key);
        if (proxy.get() == nullptr )
        {
            LOG_DBG("No proxy [ %s ] found, creating one in thread [ %u ]", ProxyAddress::to_path(Key).as_string(), ownerThread.id());
            std::shared_ptr<ProxyBase> newProxy{ funcCreate( roleName, &ownerThread ) };
            if ( newProxy.get() != nullptr )
            {
                proxy.swap( newProxy );
                map_proxies().register_resource_object( proxy->mProxyAddress, proxy );
                thread_proxies().register_resource_object( proxy->mDispatcherThread.name( ), proxy );
            }
        }

        if (proxy.get() != nullptr)
        {
            if (proxy->mListConnect.add_if_unique(&connect))
            {
                LOG_DBG("Add Service Connect notification for client [ %p ]", &connect);

                static_cast<void>(proxy->add_listener( static_cast<uint32_t>(NEService::FuncIdRange::ResponseServiceProviderConnection)
                                                    , NEService::SEQUENCE_NUMBER_NOTIFY
                                                    , static_cast<NotificationConsumer *>(&connect), true ));
                ++ proxy->mProxyInstCount;
                proxy->mIsStopped = false;

                if ( proxy->mProxyInstCount == 1 )
                {
                    proxy->register_service_listeners( );
                    ServiceManager::request_register_client( proxy->proxy_address( ) );
                }
                else if ( proxy->is_connected() )
                {
                    proxy->send_available_event( proxy->service_available_event(connect) );
                }
            }
            else
            {
                LOG_WARN("The client [ %p ] is already registered for service connection notification", &connect);
            }
        }
    }

    return proxy;
}


int32_t ProxyBase::find_thread_proxies(DispatcherThread & ownerThread, ArrayList<std::shared_ptr<ProxyBase>> & threadProxyList )
{
    ThreadProxyList * proxyList = ProxyBase::thread_proxies().find_resource(ownerThread.name());
    int32_t result = proxyList != nullptr ? static_cast<int32_t>(proxyList->size()) : 0;
    if ( result > 0 )
    {
        threadProxyList = static_cast<const ArrayList<std::shared_ptr<ProxyBase>> &>(*proxyList);
    }

    return result;
}

RemoteResponseEvent * ProxyBase::request_failure_event(const ProxyAddress & target, uint32_t msgId, NEService::ResultType errCode, const SequenceNumber & seqNr)
{
    LOG_SCOPE(areg_component_ProxyBase_createRequestFailureEvent);

    RemoteResponseEvent * result = nullptr;
    std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy(target);
    if (proxy.get() != nullptr)
    {
        result = proxy->request_failed_event(target, msgId, errCode, seqNr);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyBase::ProxyBase(const String & roleName, const NEService::InterfaceData & serviceIfData, DispatcherThread * ownerThread /*= nullptr*/ )

    : ProxyEventConsumer  ( mProxyAddress )

    , mProxyAddress     ( serviceIfData, roleName, (ownerThread != nullptr) && (ownerThread->is_valid()) ? ownerThread->name() : String::empty_string() )
    , mStubAddress      ( StubAddress::invalid_stub_address() )
    , mSequenceCount    ( 0 )
    , mListenerList     ( serviceIfData.idAttributeCount + serviceIfData.idResponseCount )
    , mListConnect      (   )
    , mProxyInstCount   ( 0 )

    , mIsStopped        ( false )

    , mProxyData        ( serviceIfData )

    , mDispatcherThread ( (ownerThread != nullptr) && (ownerThread->is_valid()) ? *ownerThread : DispatcherThread::dispatcher_thread( mProxyAddress.thread()) )
    , mConnectionStatus ( NEService::ServiceConnectionState::Unknown )
    , mIsConnected      ( false )
{
    ASSERT(mDispatcherThread.is_valid());
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, methods
//////////////////////////////////////////////////////////////////////////

void ProxyBase::register_service_listeners()
{
    ProxyConnectEvent::add_listener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::unregister_service_listeners()
{
    ProxyConnectEvent::remove_listener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
    ProxyBase::ServiceAvailableEvent::remove_listener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::free_proxy( ProxyListener & connect )
{
    int32_t exists = mListConnect.find(&connect, 0);
    if ( exists >= 0 )
    {
        mListConnect.remove_at(static_cast<uint32_t>(exists));
        connect.service_connected(NEService::ServiceConnectionState::Disconnected, self());
    }

    remove_listener( static_cast<uint32_t>(NEService::FuncIdRange::ResponseServiceProviderConnection)
                  , NEService::SEQUENCE_NUMBER_NOTIFY
                  , static_cast<NotificationConsumer *>(&connect));

    std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy( mProxyAddress );

    if ((proxy.use_count() == 1) || (mProxyInstCount == 1))
    {
        if ( false == mIsStopped )
        {
            stop_all_notifications( );
            unregister_service_listeners( );
            mListenerList.clear();

            ServiceManager::request_unregister_client( proxy_address( ), NEService::DisconnectReason::ConsumerDisconnected );
            mDispatcherThread.remove_consumer( *this );
        }

        set_connection_status( NEService::ServiceConnectionState::Disconnected );
        mIsStopped   = true;

        mProxyInstCount = 0;
        map_proxies().unregister_resource_object(mProxyAddress);
        thread_proxies().unregister_resource_object( mDispatcherThread.name( ), proxy, true );
    }
    else if ( mProxyInstCount > 0 )
    {
        -- mProxyInstCount;
    }
}

void ProxyBase::terminate_self()
{
    if (mProxyInstCount != 0)
    {
        mListenerList.clear();
        if (mIsStopped == false)
        {
            ServiceManager::request_unregister_client(proxy_address(), NEService::DisconnectReason::ConsumerDisconnected );
        }

        set_connection_status( NEService::ServiceConnectionState::Disconnected );
        mIsStopped      = true;
        mProxyInstCount = 0;

        delete this;
    }
}

void ProxyBase::service_connection_updated( const StubAddress & server, const Channel & channel, NEService::ServiceConnectionState status )
{
    LOG_SCOPE(areg_component_ProxyBase_serviceConnectionUpdated);

    ASSERT(server.is_valid());
    if ( server.is_proxy_compatible( proxy_address() ) )
    {
        LOG_DBG("The proxy [ %s ] have got [ %s ] status update notification event from stub [ %s ]"
                    , ProxyAddress::to_path(proxy_address()).as_string()
                    , NEService::as_string(status)
                    , StubAddress::to_path(server).as_string());

        ASSERT(channel.target() == server.source() || status != NEService::ServiceConnectionState::Connected);
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

        // first collect listeners, because on connect / disconnect
        // the listener list might be updated!
        ArrayList<ProxyBase::Listener> conListeners;
        uint32_t index = 0;
        for (index = 0 ; index < mListenerList.size(); ++ index)
        {
            ProxyBase::Listener& listener = mListenerList[index];
            if ( NEService::is_connect_id(listener.mMessageId) )
            {
                ASSERT(listener.mListener != nullptr);
                conListeners.add(listener);
            }
        }

        LOG_DBG("Notifying [ %d ] clients the service connection", conListeners.size());

        for (index = 0 ; index < conListeners.size(); ++ index)
        {
            const ProxyBase::Listener& listener = conListeners[index];
            ProxyListener * connect = static_cast<ProxyListener *>(listener.mListener);
            if ( proxyConnected )
            {
                mListConnect.add_if_unique(connect);
                connect->service_connected( status, *this );
            }
            else
            {
                mListConnect.remove_elem(connect, 0);
                connect->service_connected( status, *this );
                mListenerList.add_if_unique(listener);
            }
        }
    }
}

void ProxyBase::set_notification( uint32_t msgId, NotificationConsumer* caller, bool alwaysNotify /*= false*/ )
{
    if (is_connected())
    {
        bool hasListener{ has_notification_listener(msgId) };
        if ( add_listener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, caller, hasListener) )
        {
            // new listener, if attribute, send actual data.
            if (NEService::is_attribute_id(msgId))
            {
                send_notification_event( msgId
                                     , mProxyData.attribute_state(msgId) == NEService::DataState::DataIsOK ? NEService::ResultType::DataOK : NEService::ResultType::DataInvalid
                                     , NEService::SEQUENCE_NUMBER_NOTIFY, caller);
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
            if ( NEService::is_attribute_id(msgId) )
            {
                send_notification_event( msgId
                                     , mProxyData.attribute_state(msgId) == NEService::DataState::DataIsOK ? NEService::ResultType::DataOK : NEService::ResultType::DataInvalid
                                     , NEService::SEQUENCE_NUMBER_NOTIFY, caller);
            }
            else if ( NEService::is_response_id(msgId) && (mProxyData.param_state(msgId) == NEService::DataState::DataIsOK) )
            {
                send_notification_event(  msgId, NEService::ResultType::RequestOK, NEService::SEQUENCE_NUMBER_NOTIFY, caller);
            }
            else
            {
                // ignore
            }
        }
    }
    else
    {
        // ignore
    }
}

void ProxyBase::clear_notification( uint32_t msgId, NotificationConsumer* caller )
{
    remove_listener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, caller);
    if (has_notification_listener(msgId) == false)
    {
        stop_notification(msgId);
        mProxyData.set_data_state(msgId, NEService::DataState::DataIsUnavailable);
    }
}

void ProxyBase::unregister_listener( NotificationConsumer *consumer )
{
    LOG_SCOPE(areg_component_ProxyBase_unregisterListener);
    LOG_DBG("Unregisters proxy client [ %p ]", consumer);

    uint32_t index = 0;
    while (index < mListenerList.size())
    {
        const ProxyBase::Listener& elem = mListenerList[index];
        if (elem.mListener == consumer)
        {
            uint32_t msgId = elem.mMessageId;
            mListenerList.remove_at(index);
            LOG_DBG("Removes proxy client listener of message [ %u ] at index [ %d ]", msgId, index);

            if (has_notification_listener(msgId) == false)
            {
                stop_notification(msgId);
                mProxyData.set_data_state(msgId, NEService::DataState::DataIsUnavailable);
            }
        }
        else
        {
            index ++;
        }
    }
}

uint32_t ProxyBase::prepare_listeners( ProxyBase::ProxyListenerList& out_listenerList, uint32_t msgId, const SequenceNumber & seqNrToSearch )
{
    LOG_SCOPE(areg_component_ProxyBase_prepareListeners);
    ProxyBase::Listener searchListener(msgId, NEService::SEQUENCE_NUMBER_ANY);
    for (uint32_t i = 0; i < mListenerList.size(); ++ i )
    {
        const ProxyBase::Listener & elem = mListenerList.at(i);
        if ( elem == searchListener )
        {
            if ( elem.mSequenceNr == NEService::SEQUENCE_NUMBER_NOTIFY )
            {
                out_listenerList.add( elem );
            }
            else if ( elem.mSequenceNr == seqNrToSearch )
            {
                out_listenerList.add( elem );
                mListenerList.remove_at(i --);   // <== go one index back, because remove one element
            }
        }
    }

    LOG_DBG("Prepared [ %d ] proxy clients for message [ %u ] and sequence number [ %llu ]", out_listenerList.size(), msgId, seqNrToSearch);

    return out_listenerList.size();
}

void ProxyBase::notify_listeners( uint32_t respId, NEService::ResultType result, const SequenceNumber & seqNrToSearch )
{
    ProxyBase::ProxyListenerList listenerList;
    prepare_listeners(listenerList, respId, seqNrToSearch);
    for (uint32_t i = 0; i < listenerList.size(); ++ i)
    {
        const ProxyBase::Listener & elem = listenerList.at(i);
        send_notification_event(respId, result, seqNrToSearch, elem.mListener);
    }
}

void ProxyBase::send_notification_event( uint32_t msgId, NEService::ResultType resType, const SequenceNumber & seqNr, NotificationConsumer* caller )
{
    NotificationEventData data(self(), resType, msgId, seqNr);
    NotificationEvent* eventElem = create_notification_event(data);
    if (eventElem != nullptr)
    {
        if (caller != nullptr)
        {
            eventElem->set_event_consumer(static_cast<EventConsumer *>(caller));
        }

        static_cast<Event *>(eventElem)->deliver_event();
    }
}

#ifdef  DEBUG
void ProxyBase::process_proxy_event( ProxyEvent& eventElem )
{
    ASSERT(eventElem.target_proxy() == proxy_address());
}
#else   // !DEBUG
void ProxyBase::process_proxy_event( ProxyEvent& /*eventElem*/ )
{
}
#endif  // DEBUG

void ProxyBase::process_generic_event( Event& eventElem )
{
    ProxyBase::ServiceAvailableEvent * serviceEvent = AREG_RUNTIME_CAST( &eventElem, ProxyBase::ServiceAvailableEvent );
    if ( serviceEvent != nullptr )
    {
        process_available_event( serviceEvent->consumer(), serviceEvent->event_dalay() );
    }
}

std::shared_ptr<ProxyBase> ProxyBase::find_proxy( const ProxyAddress& proxyAddress )
{
    return map_proxies().find_resource_object(proxyAddress);
}

void ProxyBase::send_request_event( uint32_t reqId, const EventDataStream& args, NotificationConsumer *caller )
{
    ServiceRequestEvent* evenElem = create_request_event(args, reqId);
    if ( evenElem != nullptr )
    {
        uint32_t respId = proxy_data().response_id(static_cast<uint32_t>(reqId));
        ASSERT(NEService::is_response_id(respId) || (respId == NEService::RESPONSE_ID_NONE));

        if (respId != NEService::RESPONSE_ID_NONE)
        {
            static_cast<void>( add_listener(respId, ++ mSequenceCount, caller, true) );
            evenElem->set_sequence_number(mSequenceCount);
        }

        mProxyAddress.deliver_service_event(*evenElem);
    }
}

void ProxyBase::send_notify_request( uint32_t msgId, NEService::RequestType reqType )
{
    ServiceRequestEvent* notifyEvent = notification_request_event(msgId, reqType);
    if (notifyEvent != nullptr)
    {
        mProxyAddress.deliver_service_event( *notifyEvent );
    }
}

void ProxyBase::send_available_event( ProxyBase::ServiceAvailableEvent * eventInstance )
{
    if ( eventInstance != nullptr )
    {
        eventInstance->add_listener( self(), mDispatcherThread);
        eventInstance->set_event_consumer(this);
        eventInstance->register_for_thread(&mDispatcherThread);
        eventInstance->deliver_event( );
    }
}

bool ProxyBase::is_listener_registered( NotificationConsumer & caller ) const
{
    bool result = false;
    for (uint32_t i = 0; i < mListenerList.size(); ++i)
    {
        const ProxyBase::Listener & listener = mListenerList[i];
        if (NEService::is_connect_id(listener.mMessageId) && (&caller == listener.mListener))
        {
            result = true;
            break;
        }
    }

    return result;
}

void ProxyBase::process_available_event( NotificationConsumer & consumer, uint32_t delayEvent)
{
    if (is_connected() && is_listener_registered( consumer ) )
    {
        if (delayEvent != NECommon::DO_NOT_WAIT)
        {
            Thread::sleep(delayEvent);
        }

        static_cast<ProxyListener&>(consumer).service_connected(connection_status(), self());
    }
}

RemoteResponseEvent * ProxyBase::remote_response_event(const InStream & /* stream */) const
{
    return nullptr;
}

RemoteResponseEvent * ProxyBase::request_failed_event(  const ProxyAddress &  /* addrProxy */
                                                                , uint32_t          /* msgId */
                                                                , NEService::ResultType/* reason */
                                                                , const SequenceNumber &/* seqNr */ ) const
{
    return nullptr;
}

void ProxyBase::stop_proxy()
{
    LOG_SCOPE(areg_component_ProxyBase_stopProxy);

    if ( false == mIsStopped )
    {
        LOG_WARN("Going to stop proxy [ %s ]", ProxyAddress::to_path(mProxyAddress).as_string());

        for (uint32_t i = 0 ; i < mListConnect.size(); ++ i)
        {
            ProxyListener * listener = mListConnect.at(i);
            ASSERT(listener != nullptr);
            listener->service_connected( NEService::ServiceConnectionState::Disconnected, *this);
        }

        mListConnect.clear();

        set_connection_status( NEService::ServiceConnectionState::Disconnected );
        mIsStopped = true;

        stop_all_notifications( );
        unregister_service_listeners( );
        mListenerList.clear();
        ServiceManager::request_unregister_client( proxy_address( ), NEService::DisconnectReason::ConsumerDisconnected );
        mDispatcherThread.remove_consumer( *this );

        mStubAddress = StubAddress::invalid_stub_address();
        mProxyData.reset();
        // mProxyAddress.set_channel(Channel::invalid_channel());
    }
}
