#ifndef AREG_TESTS_UNITS_SERVICETESTSERVER_HPP
#define AREG_TESTS_UNITS_SERVICETESTSERVER_HPP

/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 ************************************************************************/

#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/service/ServiceCommunicationBase.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>

namespace service_test
{
    using Policy = areg::ext::ServiceCommunicationBase::ConnectionPolicy;

    //!< The longest wait of a test for something that must happen.
    constexpr int64_t SERVER_DEADLINE_MS{ 60'000 };

    //!< The server under test: it records the instances added and removed, so that a test waits
    //!< for them instead of sleeping.
    class TestServer final : public areg::ext::ServiceCommunicationBase
    {
    public:
        TestServer(Policy policy, uint32_t pairs, uint32_t connectTypes = static_cast<uint32_t>(areg::ConnectionType::Tcpip))
            : ServiceCommunicationBase  ( areg::COOKIE_ROUTER
                                        , areg::RemoteServiceKind::Router
                                        , connectTypes
                                        , areg::SYSTEM_THREAD_STACK_BIG
                                        , areg::String("TestCommServer")
                                        , policy
                                        , pairs )
            , mMutex    ( )
            , mChanged  ( )
            , mAdded    ( 0u )
            , mRemoved  ( 0u )
            , mDisconnects  ( )
        {
        }

        using areg::ext::ServiceCommunicationBase::set_close_stalled;

        //!< Returns the backlog of the shared send thread.
        areg::ext::SendBacklog & shared_backlog() noexcept
        {
            return mThreadSend.backlog();
        }

        //!< Marks the connection as stopping, as stop_connection() does, without stopping it.
        void set_stopping(bool stopping) noexcept
        {
            mShuttingDown.store(stopping, std::memory_order_release);
        }

        //!< Shrinks the send buffer of the connection of client \a cookie to \a bytes; returns false if the client is unknown.
        bool shrink_send_buffer(ITEM_ID cookie, uint32_t bytes) noexcept
        {
            const areg::Socket::SocketHandle hold{ mClientPairs.empty() ? mServerConnection.hold_by_cookie(cookie)
                                                                         : mClientPairs[static_cast<uint32_t>(cookie) % mNumPairs]->hold_by_cookie(cookie) };
            const SOCKETHANDLE handle{ hold.value() };
            if (areg::is_valid_socket(handle) == false)
                return false;

            static_cast<void>(areg::set_send_size(handle, bytes));
            return true;
        }

        //!< Returns true while the send backlog that serves client \a cookie keeps data not yet written.
        bool has_unwritten_data(ITEM_ID cookie) noexcept
        {
            return ((mClientPairs.empty() ? mThreadSend.backlog() : mClientPairs[static_cast<uint32_t>(cookie) % mNumPairs]->send_thread().backlog()).is_empty() == false);
        }

        //!< Returns the backlog of the send thread of pool pair \a index; the server must run.
        areg::ext::SendBacklog & pool_backlog(uint32_t index) noexcept
        {
            return mClientPairs[index]->send_thread().backlog();
        }

        void add_instance(const ITEM_ID & cookie, const areg::ConnectedInstance & instance) override
        {
            areg::ext::ServiceCommunicationBase::add_instance(cookie, instance);
            std::lock_guard<std::mutex> guard(mMutex);
            ++mAdded;
            mChanged.notify_all();
        }

        void remove_instance(const ITEM_ID & cookie) override
        {
            areg::ext::ServiceCommunicationBase::remove_instance(cookie);
            std::lock_guard<std::mutex> guard(mMutex);
            ++mRemoved;
            mChanged.notify_all();
        }

        void failed_process_message(const areg::MessageEnvelope & /*msgFailed*/) override
        {
        }

        void on_connection_started() override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            ++mStartedCalls;
            mChanged.notify_all();
        }

        void on_connection_stopped() override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            ++mStoppedCalls;
            mChanged.notify_all();
        }

        void on_connection_lost() override
        {
            ++mLostCalls;
        }

        void on_message_received(const areg::MessageEnvelope & msgReceived) override
        {
            if (msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceDisconnect))
            {
                std::lock_guard<std::mutex> guard(mMutex);
                mDisconnects.push_back(msgReceived.source());
                mChanged.notify_all();
            }
        }

        //!< Waits until the start hook was called \a count times.
        bool wait_started(uint32_t count)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(SERVER_DEADLINE_MS), [&]() { return (mStartedCalls.load() >= count); });
        }

        //!< Waits until the stop hook was called \a count times.
        bool wait_stopped(uint32_t count)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(SERVER_DEADLINE_MS), [&]() { return (mStoppedCalls.load() >= count); });
        }

        //!< Waits until at least \a count instances were added.
        bool wait_added(uint32_t count)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(SERVER_DEADLINE_MS), [&]() { return (mAdded >= count); });
        }

        //!< Waits until at least \a count instances were removed.
        bool wait_removed(uint32_t count)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(SERVER_DEADLINE_MS), [&]() { return (mRemoved >= count); });
        }

        //!< Waits until exactly \a count instances are connected.
        bool wait_instances(uint32_t count)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(SERVER_DEADLINE_MS), [&]() { return (instances().size() == count); });
        }

        //!< Waits until the disconnect message of the client with \a cookie was delivered.
        bool wait_disconnect(ITEM_ID cookie)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(SERVER_DEADLINE_MS), [&]()
                {
                    return (std::find(mDisconnects.begin(), mDisconnects.end(), static_cast<uint32_t>(cookie)) != mDisconnects.end());
                });
        }

    private:
        std::mutex              mMutex;
        std::condition_variable mChanged;
        uint32_t                mAdded;
        uint32_t                mRemoved;
        std::vector<uint32_t>   mDisconnects;

    public:
        std::atomic<uint32_t>   mStartedCalls{ 0u };
        std::atomic<uint32_t>   mStoppedCalls{ 0u };
        std::atomic<uint32_t>   mLostCalls{ 0u };
    };
}

#endif  // AREG_TESTS_UNITS_SERVICETESTSERVER_HPP
