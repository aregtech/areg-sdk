#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_POOLSENDTHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_POOLSENDTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/PoolSendThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool send thread serving a set of clients.
 *              One instance is created per pool slot; it resolves the target
 *              socket at send time via client_by_cookie() so it can serve
 *              multiple clients simultaneously. Stats are accumulated into
 *              the global ServerSendThread so DataRateHelper sees correct totals.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventConsumer.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessageHandler;
} // namespace areg

namespace areg::ext {
    class ServerConnection;
    class ServerSendThread;
    class ClientConnectionPair;
} // namespace areg::ext

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// PoolSendThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Pool send thread: serves all clients routed to this pool slot.
 *          The target socket is resolved on each send by cookie lookup, so
 *          no per-client state is stored in the thread. Stats are forwarded
 *          to the global ServerSendThread for DataRateHelper aggregation.
 **/
class PoolSendThread final  : public    DispatcherThread
                            , public    areg::EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    using BatchEntries = std::array<areg::ext::PendingSend, areg::DEFAULT_DRAIN_LIMIT>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a pool send thread.
     *
     * \param   remoteService   Remote message handler (for failure callbacks).
     * \param   connection      Server connection object (owns the socket map).
     * \param   globalStats     The global send thread whose atomic counters accumulate
     *                          the bytes/messages contributed by this thread.
     * \param   threadName      Unique name for this dispatcher thread.
     **/
    PoolSendThread( ClientConnectionPair & owner
                    , areg::RemoteMessageHandler & remoteService
                    , ServerConnection & connection
                    , ServerSendThread & globalStats
                    , std::string_view threadName );

    virtual ~PoolSendThread() = default;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables / disables the inherited DispatcherThread event loop.
     *
     * \param   is_ready    True when the thread is ready to receive events; false on shutdown.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// EventRouter overrides
/************************************************************************/

    /**
     * \brief   Posts the event to this thread's queue.
     *
     * \param   eventElem   Event to post.
     * \return  Returns true if the event was accepted and queued.
     **/
    bool post_event( Event & eventElem ) final;

private:
/************************************************************************/
// EventConsumer interface override.
/************************************************************************/

    /**
     * \brief   Receives IPC outbound events and exit signals dispatched to this send thread.
     *          Zeros internal1/internal2/custom before wire transmission.
     *          Exits on is_exit_prio().
     **/
    void start_event_processing( areg::Event & eventElem ) final;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ClientConnectionPair&       mOwner;         //!< Reference to the owning connection pair for socket lookup.
    areg::RemoteMessageHandler& mRemoteService; //!< Failure callbacks.
    ServerConnection &          mConnection;    //!< Server connection (socket lookup + send API).
    ServerSendThread &          mGlobalStats;   //!< Global counters accumulated here.
    BatchEntries                mBatch;         //!< Pre-allocated batch work list reused each drain cycle.
    //!< Reused scratch: per-slot target cookies and resolved socket handles (POD; off the stack).
    std::array<ITEM_ID, areg::DEFAULT_DRAIN_LIMIT>       mTargets;
    std::array<SOCKETHANDLE, areg::DEFAULT_DRAIN_LIMIT>  mSockets;
    //!< Reusable single-window drain buffer (pop_events); constructed once.
    std::array<areg::Event, areg::DEFAULT_DRAIN_LIMIT>   mEvents;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PoolSendThread() = delete;
    AREG_NOCOPY_NOMOVE( PoolSendThread );
};

//////////////////////////////////////////////////////////////////////////
// PoolSendThread inline methods
//////////////////////////////////////////////////////////////////////////

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_POOLSENDTHREAD_HPP
