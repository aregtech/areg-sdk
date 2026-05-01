#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServerSendThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service connectivity server send message thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/SendMessageEvent.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include <array>
#include <atomic>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessageHandler;
} // namespace areg

namespace areg::ext {
    class ServerConnection;
} // namespace areg::ext

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message sender thread.
 **/
class ServerSendThread final    : public    DispatcherThread
                                , public    SendMessageEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

    using BatchEntries = std::array<areg::PendingSend, areg::THREAD_BATCH_LIMIT>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection servicing handler and server connection objects.
     *
     * \param   remoteService       The instance of remote servicing handle to set.
     * \param   connection          The instance of server socket connection object.
     **/
    ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection );

    virtual ~ServerSendThread() = default;

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns accumulative value of sent data size and resets the existing value to zero.
     *          The operations are atomic. The value can be used to display data rate, for example.
     **/
    [[nodiscard]]
    inline uint64_t extract_data_send() const noexcept;

    /**
     * \brief   Returns accumulative count of sent messages and resets the existing value to zero.
     *          The operations are atomic.
     **/
    [[nodiscard]]
    inline uint32_t extract_msgs_sent() const noexcept;

    /**
     * \brief   Call to enable or disable the received data calculation. It also resets the existing
     *          calculated data.
     *
     * \param   enable      Flag, indicating whether data calculation is enabled or not.
     **/
    inline void set_data_rate_enabled(bool enable) noexcept;

    [[nodiscard]]
    inline bool is_data_rate_enabled() const noexcept;

    /**
     * \brief   Accumulates bytes and message counts from a per-client send thread into the
     *          global counters queried by DataRateHelper. Called by PoolSendThread when
     *          mSaveDataSend is enabled. Thread-safe: uses atomic add.
     *
     * \param   bytes   Number of bytes sent.
     * \param   msgs    Number of messages sent.
     **/
    inline void accumulate_sent(uint64_t bytes, uint32_t msgs) noexcept;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events. Override if
     *          need to make event dispatching preparation job.
     *
     * \param   is_ready    The flag to indicate whether the dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// EventRouter class overrides
/************************************************************************/

    /**
     * \brief   Posts event and delivers to its target. Since the Dispatcher Thread is a Base object
     *          for Worker and Component threads, it does nothing and only destroys event object
     *          without processing. Override this method or use Worker / Component thread.
     *
     * \param   eventElem       Event object to post
     * \return  In this class it always returns true.
     **/
    [[nodiscard]]
    bool post_event( Event & eventElem ) final;

private:
/************************************************************************/
// SendMessageEventConsumer class overrides.
/************************************************************************/
    /**
     * \brief   Automatically triggered when event is dispatched by registered worker / component
     *          thread.
     *
     * \param   data    The data object passed in event. It should have at least default constructor
     *                  and assigning operator. This object is not used for IPC.
     **/
    void process_event( const SendMessageEventData & data ) final;

    /**
     * \brief   Looks up the target client by cookie, sends the message, and updates byte counter.
     *          Returns true on success, false if the socket reported an error.
     *
     * \param   msg     Remote message to send.
     **/
    bool _do_send( const RemoteMessage & msg );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote servicing interface object
     **/
    RemoteMessageHandler&           mRemoteService;
    /**
     * \brief   The instance of server connection object
     **/
    ServerConnection &              mConnection;
    /**
     * \brief   Pre-allocated batch work list reused across every drain cycle.
     **/
    BatchEntries                    mBatch;
    /**
     * \brief   Accumulative value of sent data size.
     **/
    mutable std::atomic_uint64_t    mBytesSend;
    /**
     * \brief   Accumulative count of sent messages.
     **/
    mutable std::atomic_uint32_t    mMsgsSend;
    /**
     * \brief   Flag, indicating whether should calculate send data size or not. By default it does not compute.
     **/
    bool                            mSaveDataSend;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerSendThread() = delete;
    AREG_NOCOPY_NOMOVE( ServerSendThread );
};

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class inline methods
//////////////////////////////////////////////////////////////////////////

inline uint64_t ServerSendThread::extract_data_send() const noexcept
{
    return mBytesSend.exchange(0u, std::memory_order_relaxed);
}

inline uint32_t ServerSendThread::extract_msgs_sent() const noexcept
{
    return mMsgsSend.exchange(0u, std::memory_order_relaxed);
}

inline void ServerSendThread::set_data_rate_enabled(bool enable) noexcept
{
    if (mSaveDataSend != enable)
    {
        mBytesSend.store(0u, std::memory_order_relaxed);
        mMsgsSend.store(0u, std::memory_order_relaxed);
        mSaveDataSend = enable;
    }
}

inline bool ServerSendThread::is_data_rate_enabled() const noexcept
{
    return mSaveDataSend;
}

inline void ServerSendThread::accumulate_sent(uint64_t bytes, uint32_t msgs) noexcept
{
    if (mSaveDataSend)
    {
        mBytesSend.fetch_add(bytes, std::memory_order_relaxed);
        mMsgsSend.fetch_add(msgs, std::memory_order_relaxed);
    }
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
