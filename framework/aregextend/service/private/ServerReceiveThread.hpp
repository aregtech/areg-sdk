#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERRECEIVETHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERRECEIVETHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServerReceiveThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service connectivity server receive message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"

#include <atomic>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessage;
    class RemoteMessageHandler;
    class SocketAddress;
} // namespace areg

namespace areg::ext {
    class ConnectionHandler;
    class ServerConnection;
}

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// ServerConnection class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message receiving thread of server socket.
 **/
class ServerReceiveThread final : public    DispatcherThread
{
    //!< Number of retries to accept socket connection
    static constexpr uint32_t RETRY_COUNT   { 5 };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection, connection servicing and connection handling objects.
     *
     * \param   connectHandler      The instance of server socket connect / disconnect handling
     *                              interface
     * \param   remoteService       The instance of remote servicing handler
     * \param   connection          The instance of server connection object.
     **/
    ServerReceiveThread( ConnectionHandler & connectHandler, RemoteMessageHandler& remoteService, ServerConnection & connection );
    virtual ~ServerReceiveThread() = default;

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns accumulative value of received data size and resets the existing value to
     *          zero. The operations are atomic. The value can be used to display data rate, for example.
     **/
    [[nodiscard]]
    inline uint64_t extract_data_receive() const noexcept;

    /**
     * \brief   Returns accumulative count of received messages and resets the existing value to zero.
     *          The operations are atomic.
     **/
    [[nodiscard]]
    inline uint32_t extract_msgs_received() const noexcept;

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
     * \brief   Accumulates bytes and message counts from a per-client receive thread into the
     *          global counters queried by DataRateHelper. Called by PoolReceiveThread when
     *          mSaveDataReceive is enabled. Thread-safe: uses atomic add.
     *
     * \param   bytes   Number of bytes received.
     * \param   msgs    Number of messages received.
     **/
    inline void accumulate_received(uint64_t bytes, uint32_t msgs) noexcept;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Triggered when dispatcher starts running. In this function runs main dispatching
     *          loop. Events are picked and dispatched here. Override if logic should be changed.
     *
     * \return  Returns true if Exit Event is signaled.
     **/
    bool run_dispatcher() final;

//////////////////////////////////////////////////////////////////////////
// Hidden helpers
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Processes one socket event identified by \a hSocket.
     *          Handles both new connection acceptance and data reception from
     *          an already-accepted client. On new connections, \a addrAccepted
     *          must contain the peer address returned by wait_connection().
     *
     * \param   hSocket         The ready socket handle returned by wait_connection().
     * \param   addrAccepted    Peer address; meaningful only for new (not-yet-accepted)
     *                          connections.
     * \param   msgReceived     Reusable message buffer. Caller must call invalidate()
     *                          after this method returns.
     **/
    void _process_connection_event(SOCKETHANDLE hSocket, const areg::SocketAddress & addrAccepted, areg::RemoteMessage & msgReceived);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Service connection handler.
     **/
    ConnectionHandler&          mConnectHandler;
    /**
     * \brief   The instance of remote service message handler.
     **/
    RemoteMessageHandler &      mRemoteService;
    /**
     * \brief   The instance of server connection object
     **/
    ServerConnection &          mConnection;
    /**
     * \brief   Accumulative value of received data size.
     */
    mutable std::atomic_uint64_t    mBytesReceive;
    /**
     * \brief   Accumulative count of received messages.
     **/
    mutable std::atomic_uint32_t    mMsgsReceive;
    /**
     * \brief   Flag, indicating whether data calculation is enabled or disabled. By default, it is disabled.
     **/
    bool                            mSaveDataReceive;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerReceiveThread() = delete;
    AREG_NOCOPY_NOMOVE( ServerReceiveThread );
};

//////////////////////////////////////////////////////////////////////////
// ServerConnection inline methods.
//////////////////////////////////////////////////////////////////////////

inline uint64_t ServerReceiveThread::extract_data_receive() const noexcept
{
    return mBytesReceive.exchange(0, std::memory_order_relaxed);
}

inline uint32_t ServerReceiveThread::extract_msgs_received() const noexcept
{
    return mMsgsReceive.exchange(0, std::memory_order_relaxed);
}

inline void ServerReceiveThread::set_data_rate_enabled(bool enable) noexcept
{
    if (mSaveDataReceive != enable)
    {
        mBytesReceive.store(0u, std::memory_order_relaxed);
        mMsgsReceive.store(0u, std::memory_order_relaxed);
        mSaveDataReceive = enable;
    }
}

inline bool ServerReceiveThread::is_data_rate_enabled() const noexcept
{
    return mSaveDataReceive;
}

inline void ServerReceiveThread::accumulate_received(uint64_t bytes, uint32_t msgs) noexcept
{
    if (mSaveDataReceive)
    {
#if 1
        mBytesReceive.fetch_add(bytes, std::memory_order_relaxed);
        mMsgsReceive.fetch_add(msgs, std::memory_order_relaxed);
#else
        mBytesReceive += bytes;
        mMsgsReceive += msgs;
#endif
    }
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERRECEIVETHREAD_HPP
