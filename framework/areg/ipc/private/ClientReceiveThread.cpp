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
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_ipc_private_ClientReceiveThread, run_dispatcher);

ClientReceiveThread::ClientReceiveThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String & namePrefix)
    : DispatcherThread  (namePrefix + areg::CLIENT_RECEIVE_MESSAGE_THREAD, areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM)
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesReceive     ( 0 )
    , mSaveDataReceive  ( false )
{
}

bool ClientReceiveThread::run_dispatcher()
{
    LOG_SCOPE( areg_ipc_private_ClientReceiveThread, run_dispatcher );
    LOG_DBG("Starting client service dispatcher thread [ %s ]", name().as_string());
    
    ready_for_events( true );

    SyncObject* syncObjects[2] {&mEventExit, &mEventQueue};
    MultiLock multiLock(syncObjects, 2, false);
    RemoteMessage msgReceived;
    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Error) };

    do
    {
        whichEvent = multiLock.lock(areg::DO_NOT_WAIT, false);
        if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
        {
            whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue); // escape quit
            int32_t sizeReceive = mConnection.receive_message( msgReceived );
            if ( sizeReceive <= 0 )
            {
                msgReceived.invalidate();
                mRemoteService.failed_receive_message( mConnection.socket() );
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);
            }
            else
            {
                if (mSaveDataReceive)
                {
                    mBytesReceive += static_cast<uint32_t>(sizeReceive);
                }

                mRemoteService.process_received_message( msgReceived, mConnection.socket( ) );
            }

            msgReceived.invalidate();
        }
        else
        {
            Event * eventElem = whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ? pick_event() : nullptr;
            whichEvent = is_exit_event(eventElem) ? static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit) : whichEvent;
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
