/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientReceiveThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Receive Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientReceiveThread.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_ipc_private_ClientReceiveThread, run_dispatcher);

ClientReceiveThread::ClientReceiveThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String & namePrefix)
    : DispatcherThread  (namePrefix + areg::CLIENT_RECEIVE_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::QUEUE_SIZE_MAXIMUM)
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mRecvStats        ( )
{
}

bool ClientReceiveThread::run_dispatcher()
{
    LOG_SCOPE( areg_ipc_private_ClientReceiveThread, run_dispatcher );
    LOG_DBG("Starting client service dispatcher thread [ %s ]", name().as_string());

    // Client receive thread serves one socket.
    areg::set_receive_mode(areg::ReceiveMode::MonoCache);

    ready_for_events( true );

#ifdef USE_FAST_EVENT
    SpinSyncEvent* events[2] { &mEventExit, &mEventQueue };
#else
    SyncObject* syncObjects[2] {&mEventExit, &mEventQueue};
    MultiLock multiLock(syncObjects, 2, false);
#endif  // USE_FAST_EVENT
    RemoteMessage msgReceived;
    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Error) };

    constexpr uint32_t DRAIN_LIMIT{ areg::THREAD_DRAIN_LIMIT };
    uint32_t drainCount{ 0u };

    do
    {
#ifdef USE_FAST_EVENT
        whichEvent = drainCount == 0u ? SpinSyncEvent::wait_any(events, 2, areg::DO_NOT_WAIT) : SpinSyncEvent::WAIT_INDEX_TIMEOUT;
        if ( whichEvent == SpinSyncEvent::WAIT_INDEX_TIMEOUT )
#else
        whichEvent = drainCount == 0u ? multiLock.lock(areg::DO_NOT_WAIT, false) : MultiLock::LOCK_INDEX_TIMEOUT;
        if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
#endif  // USE_FAST_EVENT
        {
            whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue); // escape quit
            int32_t sizeReceive = mConnection.receive_message( msgReceived );
            if ( sizeReceive <= 0 )
            {
                mRemoteService.failed_receive_message( mConnection.socket() );
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);
                drainCount = 0u;
            }
            else
            {
                accumulate_received(static_cast<uint64_t>(sizeReceive), 1);
                mRemoteService.process_received_message( msgReceived, mConnection.socket( ) );
                drainCount = (drainCount + 1) % DRAIN_LIMIT;
            }
        }
        else
        {
            Event * eventElem = whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ? pick_event() : nullptr;
            whichEvent = is_exit_event(eventElem) ? static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit) : whichEvent;
            drainCount = 0;
        }

    } while (whichEvent == static_cast<int>(EventDispatcherBase::EventSignal::Queue));

    ready_for_events(false);
    remove_all_events( );

    LOG_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]"
                , name().as_string()
                , whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit) ? "SUCCESS" : "FAILURE");

    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

} // namespace areg
