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

#include <atomic>

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
     * \brief   Returns accumulated received data size and resets counter atomically. Useful for
     *          displaying data rate.
     **/
    [[nodiscard]]
    inline uint64_t extract_data_receive() const noexcept;

    /**
     * \brief   Enables or disables received data calculation and resets existing calculated data.
     *
     * \param   enable      Flag indicating whether data calculation should be enabled.
     **/
    inline void set_data_rate_enabled(bool enable) noexcept;

    /**
     * \brief   Returns whether data calculation is enabled.
     **/
    [[nodiscard]]
    inline bool is_data_rate_enabled() const noexcept;

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
     * \brief   Accumulative value of received data size.
     */
    mutable std::atomic_uint64_t    mBytesReceive;

    /**
     * \brief   Flag, indicating whether data calculation is enabled or disabled. By default, it is disabled.
     **/
    bool                            mSaveDataReceive;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientReceiveThread() = delete;
    AREG_NOCOPY_NOMOVE( ClientReceiveThread );
};

inline uint64_t ClientReceiveThread::extract_data_receive() const noexcept
{
    return static_cast<uint64_t>(mBytesReceive.exchange(0));
}

inline void ClientReceiveThread::set_data_rate_enabled(bool enable) noexcept
{
    if (mSaveDataReceive != enable)
    {
        mBytesReceive.store(0u);
        mSaveDataReceive = enable;
    }
}

inline bool ClientReceiveThread::is_data_rate_enabled() const noexcept
{
    return mSaveDataReceive;
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
