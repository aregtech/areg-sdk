/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/PoolSendThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool send thread implementation.
 ************************************************************************/
#include "aregextend/service/private/PoolSendThread.hpp"

#include "areg/component/ExitEvent.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServerSendThread.hpp"
#include "aregextend/service/private/ClientConnectionPair.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"

#include "areg/base/private/DebugDefs.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolSendThread, start_event_processing);
DEF_LOG_SCOPE(areg_aregextend_service_PoolSendThread, backlog_close);

PoolSendThread::PoolSendThread( ClientConnectionPair & owner
                                , areg::RemoteMessageHandler & remoteService
                                , ServerConnection & connection
                                , ServerSendThread & globalStats
                                , std::string_view threadName )
    : DispatcherThread  ( String(threadName), areg::SYSTEM_THREAD_STACK_BIG, areg::SEND_THREAD_QUEUE_LIMIT )
    , EventConsumer     ( )

    , mOwner            ( owner )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mGlobalStats      ( globalStats )
    , mDrainLimit       ( areg::DEFAULT_DRAIN_LIMIT )
    , mBatch            ( )
    , mSendGate         ( )
    , mBacklog          ( mSendGate )
{
}

void PoolSendThread::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        mDrainLimit = areg::send_batch_limit();
        areg::set_receive_mode(areg::ReceiveMode::MonoCache);
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        release_backlog();
    }
}

void PoolSendThread::start_event_processing( areg::Event & eventElem )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolSendThread, start_event_processing);
    AREG_LT_SCOPE(areg::LtStage::SendNode);     // drain + resolve + sort + writev

    const areg::ext::SendBatchContext context{ mBatch.data()
                                             , mEvents.data()
                                             , mTargets.data()
                                             , mSockets.data()
                                             , &mSendGate
                                             , &mConnection
                                             , &mRemoteService
                                             , &mBacklog };

    areg::ext::run_send_batch( *this
                             , eventElem
                             , context
                             , [this]( ITEM_ID * targets, SOCKETHANDLE * sockets, uint32_t count )
                               {
                                   mOwner.batch_sockets_by_cookies(targets, sockets, count);
                               }
                             , [this]( uint64_t bytes, uint32_t msgs )
                               {
                                   mGlobalStats.accumulate_sent(bytes, msgs);
                               }
                             , [&]( )
                               {
                                   // A pool thread owns none of the sockets it writes into: the
                                   // connection pair it serves closes them.
                                   DEBUG_LOG_DBG("Going to quit pool send message thread");
                                   release_backlog();
                               }
                             , [&]( [[maybe_unused]] uint32_t messageId, [[maybe_unused]] ITEM_ID target )
                               {
                                   DEBUG_LOG_WARN("Discarding message (ID = [ %u ]) for disconnected target [ %u ]"
                                                   , messageId
                                                   , static_cast<uint32_t>(target));
                               } );

    areg::ext::serve_backlog(*this, mBacklog, static_cast<SendBacklog::Owner &>(*this));
}

SOCKETHANDLE PoolSendThread::backlog_socket(ITEM_ID cookie)
{
    return mOwner.socket_by_cookie(cookie);
}

void PoolSendThread::backlog_sent(uint64_t bytes, uint32_t msgs)
{
    mGlobalStats.accumulate_sent(bytes, msgs);
}

void PoolSendThread::backlog_close(const SendBacklog::Entry & entry, SendBacklog::Reason reason)
{
    LOG_SCOPE(areg_aregextend_service_PoolSendThread, backlog_close);
    mBacklog.log_close(entry, reason);
    if ( mConnection.is_interrupted() == false )
    {
        areg::SocketAccepted client{ mConnection.client_by_handle(entry.socket) };
        mRemoteService.failed_send_message(entry.messages.front(), client);
    }
}

void PoolSendThread::release_backlog()
{
    std::deque<SOCKETHANDLE> cutSockets;
    mBacklog.release_all(cutSockets);
    for ( const SOCKETHANDLE hSocket : cutSockets )
    {
        areg::SocketAccepted client{ mConnection.client_by_handle(hSocket) };
        if ( client.is_valid() )
        {
            mConnection.close_connection(client);
        }
    }
}

bool PoolSendThread::post_event( Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

} // namespace areg::ext
