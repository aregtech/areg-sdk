#ifndef AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
#define AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientReceiveThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Receive Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/DataRateStats.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessageHandler;
    class ClientConnection;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message receiver thread from remote routing service that dispatches received messages to
 *          components.
 **/
class ClientReceiveThread final : public    DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes message receiver with service handler and client connection.
     *
     * \param   remoteService       Remote service handler for processing messages.
     * \param   connection          Client connection object for reading messages.
     * \param   namePrefix          Prefix for thread name to ensure uniqueness.
     **/
    ClientReceiveThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String & namePrefix);

    virtual ~ClientReceiveThread() = default;

    /************************************************************************/
    // Actions and attributes.
    /************************************************************************/
public:
    /**
     * \brief   Returns accumulative value of received data size and resets the existing value to
     *          zero. The operations are atomic. The value can be used to display data rate, for example.
     **/
    [[nodiscard]]
    inline uint64_t extract_data_received() const noexcept;

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

    /**
     * \brief   Returns true if data and message rating is enabled.
     **/
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
     * \brief   Runs main dispatching loop. Picks and dispatches events. Override to change logic.
     *
     * \return  Returns true if exit event is signaled.
     **/
    bool run_dispatcher() final;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote service handler to dispatch messages.
     **/
    RemoteMessageHandler&     mRemoteService;
    /**
     * \brief   The instance of connection to receive messages from remote routing service.
     **/
    ClientConnection &          mConnection;

    /**
     * \brief   Atomic stats (bytes + messages received + enabled flag).
     **/
    DataRateStats                   mRecvStats;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientReceiveThread() = delete;
    AREG_NOCOPY_NOMOVE( ClientReceiveThread );
};

//////////////////////////////////////////////////////////////////////////
// ClientReceiveThread inline methods
//////////////////////////////////////////////////////////////////////////

inline uint64_t ClientReceiveThread::extract_data_received() const noexcept
{
    return mRecvStats.extract_bytes();
}

inline uint32_t ClientReceiveThread::extract_msgs_received() const noexcept
{
    return mRecvStats.extract_msgs();
}

inline void ClientReceiveThread::set_data_rate_enabled(bool enable) noexcept
{
    mRecvStats.set_enabled(enable);
}

inline bool ClientReceiveThread::is_data_rate_enabled() const noexcept
{
    return mRecvStats.is_enabled();
}

inline void ClientReceiveThread::accumulate_received(uint64_t bytes, uint32_t msgs) noexcept
{
    mRecvStats.accumulate(bytes, msgs);
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
