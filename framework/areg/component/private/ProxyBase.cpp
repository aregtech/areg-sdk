/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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

#include <cstring>
#include <functional>
#include <memory>
#include <unordered_map>

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
DEF_LOG_SCOPE(areg_component_ProxyBase, send_request_event);

namespace
{
    //!< Returns true if the client is in the list of clients notified of the connection.
    inline bool _is_notified(const ArrayList<ProxyListener *> & listConnect, const NotificationConsumer * consumer) noexcept
    {
        for (uint32_t i = 0u; i < listConnect.size(); ++i)
        {
            if (static_cast<const NotificationConsumer *>(listConnect.value_at(i)) == consumer)
                return true;
        }

        return false;
    }

    //!< The message ID of the service connection listeners.
    constexpr uint32_t  CONNECTION_MSG_ID   { static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) };
    //!< The payload of a queued client notification: the event data, then the client.
    constexpr uint32_t  NOTIFY_CLIENT_OFFSET{ static_cast<uint32_t>(sizeof(NotificationEventData)) };
    constexpr uint32_t  NOTIFY_PAYLOAD_SIZE { NOTIFY_CLIENT_OFFSET + static_cast<uint32_t>(sizeof(NotificationConsumer *)) };
    //!< The number of listeners a snapshot holds without a heap allocation.
    constexpr uint32_t  SNAPSHOT_INLINE     { 8u };

    //!< A listener copied out of the listener map; nullptr once its client is removed.
    struct SnapshotEntry
    {
        SequenceNumber          seqNr;      //!< The sequence number of the listener.
        NotificationConsumer *  client;     //!< The client to call, or nullptr.
    };

    //!< The listeners a notification loop calls, visible to the thread that runs the loop.
    struct Snapshot
    {
        const ProxyBase *   proxy   { nullptr };    //!< The proxy that notifies.
        uint32_t            msgId   { 0u };         //!< The message ID of the notification.
        SnapshotEntry *     entries { nullptr };    //!< The copied listeners.
        uint32_t            count   { 0u };         //!< The number of copied listeners.
        Snapshot *          previous{ nullptr };    //!< The snapshot of an enclosing loop.
    };

    //!< The innermost notification loop of this thread.
    thread_local Snapshot * _snapshots{ nullptr };

    //!< The storage of a snapshot: inline up to SNAPSHOT_INLINE listeners, on the heap above.
    class SnapshotBuffer
    {
    public:
        SnapshotBuffer() = default;

        //!< Returns storage for 'count' listeners.
        SnapshotEntry * reserve(uint32_t count)
        {
            if (count <= SNAPSHOT_INLINE)
                return mInline;

            mHeap.reset(new SnapshotEntry[count]);
            return mHeap.get();
        }

    private:
        SnapshotEntry                       mInline[SNAPSHOT_INLINE];   //!< The inline storage.
        std::unique_ptr<SnapshotEntry[]>    mHeap;                      //!< The heap storage, if used.

        SnapshotBuffer(const SnapshotBuffer &) = delete;
        SnapshotBuffer & operator = (const SnapshotBuffer &) = delete;
    };

    //!< Publishes a snapshot to its thread while the loop that calls its listeners runs.
    class SnapshotScope
    {
    public:
        explicit SnapshotScope(Snapshot & snapshot) noexcept
            : mHead     (_snapshots)
            , mSnapshot (snapshot)
        {
            mSnapshot.previous = mHead;
            mHead = &mSnapshot;
        }

        ~SnapshotScope()
        {
            mHead = mSnapshot.previous;
        }

    private:
        Snapshot * &    mHead;      //!< The innermost snapshot of this thread.
        Snapshot &      mSnapshot;  //!< The published snapshot.

        SnapshotScope(const SnapshotScope &) = delete;
        SnapshotScope & operator = (const SnapshotScope &) = delete;
    };

    //!< Which snapshots of a proxy a removed client is cleared from.
    enum class RetractScope
    {
          Message           //!< the snapshots of one message ID, entries of one sequence number
        , Notifications     //!< every snapshot except the service connection one
        , All               //!< every snapshot
    };

    //!< Clears the client in the snapshots of the proxy that this thread is running.
    void _retract(const ProxyBase & proxy, const NotificationConsumer * client, RetractScope scope, uint32_t msgId = 0u, const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_ANY) noexcept
    {
        for (Snapshot * snapshot{ _snapshots }; snapshot != nullptr; snapshot = snapshot->previous)
        {
            if ((snapshot->proxy != &proxy)
                || ((scope == RetractScope::Message) && (snapshot->msgId != msgId))
                || ((scope == RetractScope::Notifications) && (snapshot->msgId == CONNECTION_MSG_ID)))
            {
                continue;
            }

            for (uint32_t i = 0u; i < snapshot->count; ++i)
            {
                SnapshotEntry & entry{ snapshot->entries[i] };
                if ((entry.client == client) && ((scope != RetractScope::Message) || (entry.seqNr == seqNr)))
                {
                    entry.client = nullptr;
                }
            }
        }
    }

    //!< The queued client notifications not delivered yet, counted per proxy, client and message ID.
    class PendingNotifications
    {
    public:
        PendingNotifications() = default;

        //!< Counts one more queued notification.
        void raise(const ProxyBase * proxy, const NotificationConsumer * client, uint32_t msgId)
        {
            Lock lock(mLock);
            ++ mCounts[Key{ proxy, client, msgId }];
        }

        //!< Takes one queued notification. Returns false if none is pending.
        bool consume(const ProxyBase * proxy, const NotificationConsumer * client, uint32_t msgId)
        {
            Lock lock(mLock);
            auto pos{ mCounts.find(Key{ proxy, client, msgId }) };
            if (pos == mCounts.end())
                return false;

            if (-- pos->second == 0u)
            {
                mCounts.erase(pos);
            }

            return true;
        }

        //!< Drops the queued notifications of the client and message ID.
        void remove(const ProxyBase * proxy, const NotificationConsumer * client, uint32_t msgId)
        {
            Lock lock(mLock);
            if (mCounts.empty() == false)
            {
                mCounts.erase(Key{ proxy, client, msgId });
            }
        }

        //!< Drops the queued notifications of every client of the proxy.
        void remove_proxy(const ProxyBase * proxy)
        {
            Lock lock(mLock);
            for (auto pos{ mCounts.begin() }; pos != mCounts.end(); )
            {
                pos = (pos->first.proxy == proxy) ? mCounts.erase(pos) : std::next(pos);
            }
        }

    private:
        struct Key
        {
            const ProxyBase *               proxy;  //!< The proxy that queued the notification.
            const NotificationConsumer *    client; //!< The client to notify.
            uint32_t                        msgId;  //!< The message ID of the notification.

            bool operator == (const Key & other) const noexcept
            {
                return (proxy == other.proxy) && (client == other.client) && (msgId == other.msgId);
            }
        };

        struct KeyHash
        {
            std::size_t operator () (const Key & key) const noexcept
            {
                return std::hash<const void *>{}(key.proxy) ^ (std::hash<const void *>{}(key.client) << 1) ^ (std::hash<uint32_t>{}(key.msgId) << 2);
            }
        };

        SpinLock                                    mLock;      //!< Guards the counts.
        std::unordered_map<Key, uint32_t, KeyHash>  mCounts;    //!< The number of queued notifications per key.
    };

    //!< Returns the queued client notifications of the process. Never destroyed.
    PendingNotifications & _pending_notifications()
    {
        static PendingNotifications * _pending{ new PendingNotifications() };
        return *_pending;
    }

    //!< Delivers a queued client notification to its client only while the notification is pending.
    class NotificationRelay final : public EventConsumer
    {
    public:
        NotificationRelay() = default;

        void start_event_processing(Event & eventElem) override
        {
            const MessageEnvelope & envelope{ eventElem.envelope() };
            if (envelope.size_used() != NOTIFY_PAYLOAD_SIZE)
                return;

            const uint8_t * payload{ envelope.payload_ptr() };
            const NotificationEventData & data{ *reinterpret_cast<const NotificationEventData *>(payload) };
            NotificationConsumer * client{ nullptr };
            std::memcpy(&client, payload + NOTIFY_CLIENT_OFFSET, sizeof(client));
            if ((client != nullptr) && _pending_notifications().consume(data.service_proxy(), client, data.notify_id()))
            {
                eventElem.dispatch_self(static_cast<EventConsumer *>(client));
            }
        }
    };

    //!< Returns the consumer of every queued client notification. Never destroyed.
    EventConsumer & _notification_relay()
    {
        static NotificationRelay * _relay{ new NotificationRelay() };
        return *_relay;
    }

    //!< The payload of a queued service available event: the client, then the proxy ID.
    constexpr uint32_t  SERVICE_PROXY_OFFSET{ static_cast<uint32_t>(sizeof(NotificationConsumer *)) };
    constexpr uint32_t  SERVICE_PAYLOAD_SIZE{ SERVICE_PROXY_OFFSET + static_cast<uint32_t>(sizeof(uint32_t)) };

    //!< Delivers a queued service available event to its proxy only while the proxy is registered.
    class ServiceRelay final : public EventConsumer
    {
    public:
        ServiceRelay() = default;

        void start_event_processing(Event & eventElem) override
        {
            const MessageEnvelope & envelope{ eventElem.envelope() };
            if (envelope.size_used() != SERVICE_PAYLOAD_SIZE)
                return;

            uint32_t proxyId{ 0u };
            std::memcpy(&proxyId, envelope.payload_ptr() + SERVICE_PROXY_OFFSET, sizeof(proxyId));
            const std::shared_ptr<ProxyBase> proxy{ ProxyBase::find_proxy(proxyId) };
            if (proxy.get() != nullptr)
            {
                eventElem.dispatch_self(static_cast<EventConsumer *>(proxy.get()));
            }
        }
    };

    //!< Returns the consumer of every queued service available event. Never destroyed.
    EventConsumer & _service_relay()
    {
        static ServiceRelay * _relay{ new ServiceRelay() };
        return *_relay;
    }
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ImplProxyMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase::ServiceAvailableEvent::ServiceAvailableEvent( NotificationConsumer & consumer )
    : Event ( areg::EventType::EventCustomExternal, static_cast<uint32_t>(sizeof(NotificationConsumer *)) )
{
    // The client is in the payload; the event consumer is the proxy that processes the event.
    uint8_t * data{ payload_ptr() };
    if (data != nullptr)
    {
        *reinterpret_cast<NotificationConsumer **>(data) = &consumer;
        set_size_used(static_cast<uint32_t>(sizeof(NotificationConsumer *)));
    }
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
    DispatcherThread & owner = ownerThread.is_empty()
                                    ? DispatcherThread::current_dispatcher_thread()
                                    : DispatcherThread::dispatcher_thread(ThreadAddress(ownerThread));
    return ProxyBase::acquire_proxy(roleName, serviceIfData, connect, funcCreate, owner);
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

    ASSERT( !roleName.is_empty() );     // a proxy without a service role name is invalid
    std::shared_ptr<ProxyBase> proxy{ nullptr };
    if (!ownerThread.is_valid())
    {
        // owner dispatcher thread did not resolve (unnamed or unknown)
        ASSERT( false );
        return proxy;
    }

    if (ownerThread.id() != Thread::current_thread_id())
    {
        // A client and its proxy are created in the same thread.
        LOG_ERR("Refused to create the proxy [ %s ] of thread [ %s ] from another thread", roleName.as_string(), ownerThread.name().as_string());
        ASSERT( false );
        return proxy;
    }

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
    
    // Whether the client is already attached is answered by the connection listener list
    if (proxy->is_listener_registered(static_cast<NotificationConsumer &>(connect)))
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
    Lock lock(ProxyBase::thread_proxies().lockable());
    ThreadProxyList * proxyList = ProxyBase::thread_proxies().find_resource_locked(static_cast<uint32_t>(ownerThread.address()));
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
    return (proxy.get() != nullptr ? proxy->create_request_failed(target, msgId, errCode, seqNr) : ServiceResponseEvent(MessageEnvelope{}));
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
    ASSERT(caller != nullptr);
    if (caller == nullptr)
        return false;

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

    if (caller != nullptr)
    {
        _pending_notifications().remove(this, caller, msgId);
        _retract(self(), caller, RetractScope::Message, msgId, seqNr);
    }
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
    // A client and its proxy are deleted in the thread that created them.
    ASSERT(mDispatcherThread.id() == Thread::current_thread_id());
    _retract(self(), static_cast<NotificationConsumer *>(&connect), RetractScope::All);
    int32_t exists = mListConnect.find(&connect, 0);
    if ( exists >= 0 )
    {
        mListConnect.remove_at(static_cast<uint32_t>(exists));
        if ( mIsConnected )
            connect.service_connected(areg::ServiceConnectionState::Disconnected, self());
    }

    remove_listener( static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                   , areg::SEQUENCE_NUMBER_NOTIFY
                   , static_cast<NotificationConsumer *>(&connect));

    std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy( mProxyAddress );
    if (proxy.get() != this)
    {
        // The entry of this address belongs to another proxy.
        proxy.reset();
    }

    if ((proxy.use_count() == 1) || (mProxyInstCount == 1))
    {
        if ( !mIsStopped )
        {
            stop_all_notifications( );
            unregister_service_listeners( );
            {
                Lock lock(mListenerLock);
                mListenerMap.clear();
                _pending_notifications().remove_proxy(this);
            }

            ServiceManager::request_unregister_consumer( proxy_address( ), areg::DisconnectReason::ConsumerDisconnected );
            mDispatcherThread.remove_consumer( *this );
        }

        set_connection_status( areg::ServiceConnectionState::Disconnected );
        mIsStopped   = true;

        mProxyInstCount = 0;
        if (proxy.get() != nullptr)
        {
            map_proxies().unregister_resource_object(static_cast<uint32_t>(mProxyAddress));
            thread_proxies().unregister_resource_object( static_cast<uint32_t>(mDispatcherThread.address( )), proxy, true );
        }
    }
    else if ( mProxyInstCount > 0 )
    {
        -- mProxyInstCount;
    }
}

void ProxyBase::detach_from_registry()
{
    std::shared_ptr<ProxyBase> proxy{ ProxyBase::find_proxy(mProxyAddress) };
    if (proxy.get() == this)
    {
        map_proxies().unregister_resource_object(static_cast<uint32_t>(mProxyAddress));
        thread_proxies().unregister_resource_object( static_cast<uint32_t>(mDispatcherThread.address( )), proxy, true );
    }
}

void ProxyBase::_detach_thread_proxies( const ArrayList<String> & threads )
{
    MapProxyResource & proxies{ map_proxies() };
    for (uint32_t i = 0; i < threads.size(); ++ i)
    {
        const ThreadProxyList list{ thread_proxies().unregister_resource( static_cast<uint32_t>(ThreadAddress(threads[i])) ) };
        for (uint32_t j = 0; j < list.size(); ++ j)
        {
            const std::shared_ptr<ProxyBase> & proxy{ list[j] };
            Lock lock(proxies.lockable());
            if (proxies.find_resource_object(static_cast<uint32_t>(proxy->mProxyAddress)) == proxy)
            {
                proxies.unregister_resource_object(static_cast<uint32_t>(proxy->mProxyAddress));
            }
        }
    }
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

    const bool wasConnected{ mIsConnected };
    const bool nowConnected{ areg::is_service_connected(status) };

    // Only an exactly repeated notification may be dropped: 'Disconnected' and 'Shutdown'
    // both clear the connected flag, but only the second one is terminal.
    if ( (nowConnected == wasConnected) && (status == connection_status()) )
    {
        LOG_DBG("Ignored repeated connection update [ %s ]", areg::as_string(status));
        return;
    }

    ASSERT(channel.target() == server.source() || !server.is_local_address() || status != areg::ServiceConnectionState::Connected);
    mProxyAddress.set_channel(channel);
    set_connection_status( status );
    if (nowConnected)
    {
        mStubAddress = server;
    }
    else
    {
        mStubAddress = StubAddress::invalid_stub_address();
        mProxyData.reset();
    }

    SnapshotBuffer connectBuffer;
    Snapshot connectListeners{ this, CONNECTION_ID };
    ArrayList<uint32_t> notifyIds;
    ArrayList<uint32_t> pendingIds;
    ProxyListenerList pendingRequests;
    {
        Lock lock(mListenerLock);
        ProxyListenerMap::MAPPOS mapPos = mListenerMap.find(CONNECTION_ID);
        if (mListenerMap.is_valid_position(mapPos))
        {
            const ProxyListenerList & subVec = mapPos->second;
            connectListeners.entries = connectBuffer.reserve(subVec.size());
            for (uint32_t i = 0u; i < subVec.size(); ++i)
            {
                connectListeners.entries[i] = SnapshotEntry{ subVec.value_at(i).mSequenceNr, subVec.value_at(i).mListener };
            }

            connectListeners.count = subVec.size();
        }

        if (nowConnected && (wasConnected == false))
        {
            for (mapPos = mListenerMap.first_position(); mListenerMap.is_valid_position(mapPos); mapPos = mListenerMap.next_position(mapPos))
            {
                if ((mapPos->first != CONNECTION_ID) && has_notification_listener(mapPos->first))
                    notifyIds.add(mapPos->first);
            }
        }
        else if (wasConnected && (nowConnected == false))
        {
            for (mapPos = mListenerMap.first_position(); mListenerMap.is_valid_position(mapPos); mapPos = mListenerMap.next_position(mapPos))
            {
                const ProxyListenerList & subVec = mapPos->second;
                for (uint32_t i = 0u; i < subVec.size(); ++i)
                {
                    if (subVec.value_at(i).mSequenceNr != areg::SEQUENCE_NUMBER_NOTIFY)
                    {
                        pendingIds.add(mapPos->first);
                        pendingRequests.add(subVec.value_at(i));
                    }
                }
            }
        }
    }

    // Fails every pending request; the failures are dispatched after the connection callbacks below.
    for (uint32_t i = 0u; i < pendingIds.size(); ++i)
    {
        ServiceResponseEvent failure{ create_request_failed(mProxyAddress, pendingIds[i], areg::ResultType::ServiceUnavailable, pendingRequests[i].mSequenceNr) };
        if (failure.is_valid())
        {
            failure.deliver_event();
        }
    }

    // A provider that has just connected knows no subscription: subscribe the listeners still in the list.
    for (uint32_t i = 0u; i < notifyIds.size(); ++i)
    {
        start_notification(notifyIds[i]);
    }

    // A client removed by an earlier callback is cleared in the snapshot and skipped.
    SnapshotScope scope(connectListeners);
    for (uint32_t i = 0u; i < connectListeners.count; ++i)
    {
        ProxyListener* connect = static_cast<ProxyListener*>(connectListeners.entries[i].client);
        if (connect == nullptr)
            continue;

        if (nowConnected)
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
        if (!hasListener)
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
            if (msgId == ProxyBase::CONNECTION_ID)
            {
                // The service connection listener is owned by acquire_proxy and
                // released by free_proxy, it must not be stopped
                mapPos = mListenerMap.next_position(mapPos);
                continue;
            }

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

            if (removed)
            {
                _pending_notifications().remove(this, consumer, msgId);
                if (!has_notification_listener(msgId))  // recursive lock: safe
                    stopIds.add(msgId);
            }

            mapPos = subVec.is_empty() ? mListenerMap.remove_at(mapPos) : mListenerMap.next_position(mapPos);
        }
    }

    _retract(self(), consumer, RetractScope::Notifications);
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
    SnapshotBuffer buffer;
    Snapshot listeners{ this, respId };
    {
        Lock lock(mListenerLock);
        ProxyListenerMap::MAPPOS pos = mListenerMap.find(respId);
        if (!mListenerMap.is_valid_position(pos))
            return;

        ProxyListenerList & subVec = pos->second;
        listeners.entries = buffer.reserve(subVec.size());
        for (uint32_t i = 0; i < subVec.size(); )
        {
            const ProxyBase::Listener & elem = subVec.value_at(i);
            if (elem.mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY)
            {
                listeners.entries[listeners.count ++] = SnapshotEntry{ elem.mSequenceNr, elem.mListener };
                ++i;
            }
            else if (elem.mSequenceNr == seqNrToSearch)
            {
                listeners.entries[listeners.count ++] = SnapshotEntry{ elem.mSequenceNr, elem.mListener };
                subVec.remove_at(i);
            }
            else
            {
                ++i;
            }
        }
    }

    // A client removed by an earlier callback is cleared in the snapshot and skipped.
    SnapshotScope scope(listeners);
    for (uint32_t i = 0; i < listeners.count; ++ i)
    {
        NotificationConsumer * client{ listeners.entries[i].client };
        if (client != nullptr)
        {
            client->process_notification(respId, result, seqNrToSearch);
        }
    }
}

void ProxyBase::send_notification_event( uint32_t msgId, areg::ResultType resType, const SequenceNumber & seqNr, NotificationConsumer* caller )
{
    if (caller == nullptr)
        return;

    // The relay delivers the event to the client named in the payload.
    NotificationEventData data(self(), resType, msgId, seqNr);
    NotificationEvent eventElem{ create_client_notification(data) };
    areg::EventHeader * header{ eventElem.header() };
    if ((header == nullptr) || (header->bufHeader.biLength < NOTIFY_PAYLOAD_SIZE))
        return;

    std::memcpy(reinterpret_cast<uint8_t *>(header) + header->bufHeader.biOffset + NOTIFY_CLIENT_OFFSET, &caller, sizeof(caller));
    header->bufHeader.biUsed = NOTIFY_PAYLOAD_SIZE;
    eventElem.set_event_consumer(&_notification_relay());
    {
        Lock lock(mListenerLock);
        bool listening{ false };
        ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
        if (mListenerMap.is_valid_position(pos))
        {
            const ProxyListenerList & subVec = pos->second;
            for (uint32_t i = 0u; (i < subVec.size()) && (listening == false); ++i)
            {
                listening = (subVec.value_at(i).mListener == caller);
            }
        }

        if (listening == false)
            return;

        _pending_notifications().raise(this, caller, msgId);
    }

    if (mDispatcherThread.event_dispatcher().post_event(eventElem) == false)
    {
        static_cast<void>(_pending_notifications().consume(this, caller, msgId));
    }
}

void ProxyBase::process_proxy_event( [[maybe_unused]] ProxyEvent& eventElem )
{
    ASSERT(eventElem.target_proxy() == static_cast<uint32_t>(proxy_address()));
}

void ProxyBase::process_generic_event( Event& eventElem )
{
    const MessageEnvelope& envelope{ eventElem.envelope() };
    if ((envelope.size_used() == SERVICE_PAYLOAD_SIZE) || (envelope.size_used() == SERVICE_PROXY_OFFSET))
    {
        NotificationConsumer* consumer{ *reinterpret_cast<NotificationConsumer * const *>(envelope.payload_ptr()) };
        if (consumer != nullptr)
            process_available_event(*consumer);
    }
}

SequenceNumber ProxyBase::send_request_event(ServiceRequestEvent& reqEvent, NotificationConsumer* caller)
{
    LOG_SCOPE( areg_component_ProxyBase, send_request_event );

    if (!reqEvent.is_valid())
        return areg::SEQUENCE_NUMBER_ANY;

    const uint32_t msgId{ reqEvent.message_id() };
    const uint32_t respId{ proxy_data().response_id(msgId) };

    const bool hasResponse{ areg::is_valid_response_id(respId) };
    SequenceNumber seqNr{ areg::SEQUENCE_NUMBER_ANY };
    if (hasResponse)
    {
        Lock lock(mListenerLock);
        ++mSequenceCount;
        static_cast<void>(add_listener(respId, mSequenceCount, caller, true));  // recursive lock: safe
        reqEvent.set_sequence_number(mSequenceCount);
        seqNr = mSequenceCount;
    }

    const bool connected{ mIsConnected };
    if ((connected == false) || (mProxyAddress.deliver_service_event(reqEvent) == false))
    {
        LOG_WARN("Failed to deliver request [ %u ] of proxy [ %s ]%s%s"
                    , msgId
                    , ProxyAddress::to_path(mProxyAddress).as_string()
                    , connected ? "" : ", not connected"
                    , hasResponse ? ", failing it" : ", it expects no response");
        if (hasResponse)
        {
            const areg::ResultType reason{ connected ? areg::ResultType::MessageUndelivered : areg::ResultType::ServiceUnavailable };
            ServiceResponseEvent failure{ create_request_failed(mProxyAddress, msgId, reason, seqNr) };
            if (failure.is_valid())
            {
                failure.deliver_event();
            }
        }
    }

    return seqNr;
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

        // The relay delivers the event to the proxy whose ID is in the payload.
        areg::EventHeader * header{ eventInstance.header() };
        if ((header != nullptr) && (header->bufHeader.biUsed == SERVICE_PROXY_OFFSET) && (header->bufHeader.biLength >= SERVICE_PAYLOAD_SIZE))
        {
            const uint32_t proxyId{ static_cast<uint32_t>(mProxyAddress) };
            std::memcpy(reinterpret_cast<uint8_t *>(header) + header->bufHeader.biOffset + SERVICE_PROXY_OFFSET, &proxyId, sizeof(proxyId));
            header->bufHeader.biUsed = SERVICE_PAYLOAD_SIZE;
            eventInstance.set_event_consumer(&_service_relay());
        }
        else
        {
            eventInstance.set_event_consumer(this);
        }

        // Drop-on-failure is handled by the caller-owned eventInstance destructor.
        if (eventInstance.register_for_thread(&mDispatcherThread))
            mDispatcherThread.event_dispatcher().post_event(eventInstance);
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

void ProxyBase::process_available_event( NotificationConsumer & consumer )
{
    LOG_SCOPE(areg_component_ProxyBase, process_available_event);

    // A client is notified once, and only while it is registered.
    if (is_connected() && is_listener_registered( consumer ) && (_is_notified(mListConnect, &consumer) == false))
    {
        LOG_DBG("Notifying client [ %p ] the service connection status [ %s ]", &consumer, areg::as_string(connection_status()));
        ProxyListener& listener{ static_cast<ProxyListener&>(consumer) };
        mListConnect.add(&listener);
        listener.service_connected(connection_status(), self());
    }
}

void ProxyBase::stop_proxy()
{
    LOG_SCOPE( areg_component_ProxyBase, stop_proxy );

    if (mIsStopped)
        return;
    
    LOG_WARN("Going to stop proxy [ %s ]", ProxyAddress::to_path(mProxyAddress).as_string());
    SnapshotBuffer buffer;
    Snapshot connected{ this, CONNECTION_ID };
    if ( mIsConnected )
    {
        connected.entries = buffer.reserve(mListConnect.size());
        for (uint32_t i = 0 ; i < mListConnect.size(); ++ i)
        {
            ASSERT(mListConnect.value_at(i) != nullptr);
            connected.entries[i] = SnapshotEntry{ areg::SEQUENCE_NUMBER_NOTIFY, static_cast<NotificationConsumer *>(mListConnect.value_at(i)) };
        }

        connected.count = mListConnect.size();
    }

    mListConnect.clear();
    {
        // A client removed by an earlier callback is cleared in the snapshot and skipped.
        SnapshotScope scope(connected);
        for (uint32_t i = 0 ; i < connected.count; ++ i)
        {
            ProxyListener * listener = static_cast<ProxyListener *>(connected.entries[i].client);
            if (listener != nullptr)
            {
                listener->service_connected( areg::ServiceConnectionState::Disconnected, *this);
            }
        }
    }

    set_connection_status( areg::ServiceConnectionState::Disconnected );
    mIsStopped = true;

    stop_all_notifications( );
    unregister_service_listeners( );
    {
        Lock lock(mListenerLock);
        mListenerMap.clear();
        _pending_notifications().remove_proxy(this);
    }

    ServiceManager::request_unregister_consumer( proxy_address( ), areg::DisconnectReason::ConsumerDisconnected );
    mDispatcherThread.remove_consumer( *this );

    mStubAddress = StubAddress::invalid_stub_address();
    mProxyData.reset();
    // mProxyAddress.set_channel(Channel::invalid_channel());
}

} // namespace areg
