#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_POOLRECEIVETHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_POOLRECEIVETHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/PoolReceiveThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool receive thread serving a set of clients.
 *              One instance is created per pool slot.  It owns a SocketMultiplexer
 *              and monitors all sockets routed to its slot via epoll/kqueue/poll.
 *              Sockets are added and removed dynamically via thread-safe queues;
 *              the receive thread is interrupted by a soft wakeup() call to pick up
 *              pending registrations without a full multiplexer reset.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"

#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>
#include <string_view>
#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessageHandler;
} // namespace areg

namespace areg::ext {
    class ConnectionHandler;
    class ServerConnection;
    class ServerReceiveThread;
    class ClientConnectionPair;
} // namespace areg::ext

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// PoolReceiveThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Pool receive thread: monitors all sockets assigned to this pool slot
 *          via its own SocketMultiplexer.  Sockets are added and removed without
 *          stopping the thread; the soft wakeup() mechanism interrupts wait()
 *          long enough to register pending changes, then resumes monitoring.
 *          Stats are accumulated into the global ServerReceiveThread so
 *          DataRateHelper sees correct totals.
 **/
class PoolReceiveThread final : public DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a pool receive thread.
     *
     * \param   remoteService   Remote message handler (process / failure callbacks).
     * \param   connection      Server connection object (owns the socket map).
     * \param   globalStats     The global receive thread whose atomic counters accumulate
     *                          the bytes/messages contributed by this thread.
     * \param   threadName      Unique name for this dispatcher thread.
     **/
    PoolReceiveThread( areg::RemoteMessageHandler & remoteService
                     , ServerConnection & connection
                     , ServerReceiveThread & globalStats
                     , std::string_view threadName );

    virtual ~PoolReceiveThread() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Queues a socket for registration with this pool slot's multiplexer
     *          and delivers a soft wakeup() to the receive thread so it picks it
     *          up at the top of its next loop iteration.
     *
     * \param   clientSocket    Accepted client socket to start monitoring.
     **/
    void add_socket( const areg::SocketAccepted & clientSocket );

    /**
     * \brief   Queues a socket for removal from this pool slot's multiplexer
     *          and delivers a soft wakeup() so the receive thread processes the
     *          removal at the top of its next loop iteration.
     *
     * \param   hSocket     Handle of the socket to stop monitoring.
     **/
    void remove_socket( SOCKETHANDLE hSocket );

    /**
     * \brief   Signals the thread to stop: triggers the exit event and resets
     *          the multiplexer (unblocks any blocked wait() with FailedSocketHandle).
     *          Call instead of trigger_exit() when stopping a pool receive thread.
     **/
    void request_stop();

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Runs the multiplexer-based receive loop for all sockets in this pool slot.
     *          Returns when reset() unblocks wait() (hard stop) or an exit event is signaled.
     *
     * \return  Returns true if the dispatcher exited cleanly via an exit event or hard stop.
     **/
    bool run_dispatcher() final;

//////////////////////////////////////////////////////////////////////////
// Private helpers
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Drains the pending-add and pending-remove queues under mPendingLock
     *          and applies the changes to mMux.  Called at the top of each loop
     *          iteration before entering wait().  Uses mHasPending as a fast-path
     *          guard so the lock is not taken when no socket changes are queued.
     **/
    void _process_pending_sockets();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    areg::RemoteMessageHandler &        mRemoteService;     //!< Message processing callbacks.
    ServerConnection &                  mConnection;        //!< Server connection (for receive API).
    ServerReceiveThread &               mGlobalStats;       //!< Global counters accumulated here.
    areg::SocketMultiplexer             mMux;               //!< Per-slot socket readiness monitor.

    // JUSTIFICATION: mPendingAdd and mPendingRemove are modified from threads other than
    // the receive thread (e.g. the ServerReceiveThread calling add_socket() after accepting
    // a new connection).  mPendingLock serialises all accesses to these two queues.
    // mHasPending is a fast-path guard: the receive thread reads it without the lock to
    // skip _process_pending_sockets() entirely when no socket changes are queued.
    mutable std::atomic<bool>           mHasPending;        //!< True when mPendingAdd or mPendingRemove is non-empty.
    mutable ResourceLock                mPendingLock;       //!< Guards the two pending queues below.
    std::vector<areg::SocketAccepted>   mPendingAdd;        //!< Sockets queued for registration.
    std::vector<SOCKETHANDLE>           mPendingRemove;     //!< Socket handles queued for unregistration.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PoolReceiveThread() = delete;
    AREG_NOCOPY_NOMOVE( PoolReceiveThread );
};

//////////////////////////////////////////////////////////////////////////
// PoolReceiveThread inline methods
//////////////////////////////////////////////////////////////////////////

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_POOLRECEIVETHREAD_HPP
