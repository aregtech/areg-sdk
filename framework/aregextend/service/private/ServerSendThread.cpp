/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/ServerSendThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service connectivity server send message thread
 ************************************************************************/
#include "aregextend/service/private/ServerSendThread.hpp"

#include "areg/component/ServiceDefs.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"

#include "areg/base/private/DebugDefs.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, start_event_processing);

ServerSendThread::ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection)
    : DispatcherThread  ( areg::SERVER_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::SEND_THREAD_QUEUE_LIMIT )
    , EventConsumer     ( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mDrainLimit       ( areg::DEFAULT_DRAIN_LIMIT )
    , mBatch            ( )
    , mSendStats        ( )
    , mSendGate         ( )
{
}

void ServerSendThread::ready_for_events( bool is_ready )
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
        mConnection.close_all_connections( );
        mConnection.disable_send( );
    }
}

void ServerSendThread::start_event_processing( areg::Event & eventElem )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ServerSendThread, start_event_processing);
    AREG_LT_SCOPE(areg::LtStage::SendNode);     // drain + resolve + sort + writev

    const areg::ext::SendBatchContext context{ mBatch.data()
                                             , mEvents.data()
                                             , mTargets.data()
                                             , mSockets.data()
                                             , &mSendGate
                                             , &mConnection
                                             , &mRemoteService };

    areg::ext::run_send_batch( *this
                             , eventElem
                             , context
                             , [this]( ITEM_ID * targets, SOCKETHANDLE * sockets, uint32_t count )
                               {
                                   mConnection.batch_handles_by_cookies(targets, sockets, count);
                               }
                             , [this]( uint64_t bytes, uint32_t msgs )
                               {
                                   accumulate_sent(bytes, msgs);
                               }
                             , [&]( )
                               {
                                   // This thread owns the listening socket of the service, so it
                                   // takes the whole connection down with it.
                                   DEBUG_LOG_DBG("Going to quit send message thread");
                                   mConnection.close_all_connections();
                                   mConnection.close_socket();
                               }
                             , [&]([[maybe_unused]]  uint32_t messageId, [[maybe_unused]] ITEM_ID target )
                               {
                                   DEBUG_LOG_WARN("Discarding message (ID = [ %u ]) for disconnected target [ %u ]"
                                                   , messageId
                                                   , static_cast<uint32_t>(target));
                               } );
}

bool ServerSendThread::post_event( Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

} // namespace areg::ext
