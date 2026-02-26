/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientSendThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Send Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientSendThread.hpp"

#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(areg_ipc_private_ClientSendThread_readyForEvents);

ClientSendThread::ClientSendThread(areg::RemoteMessageHandler& remoteService, areg::ClientConnection & connection, const areg::String& namePrefix )
    : areg::DispatcherThread  ( namePrefix + areg::CLIENT_SEND_MESSAGE_THREAD, areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM )
    , areg::SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesSend        ( 0 )
    , mSaveDataSend     ( false )
{
}

void ClientSendThread::readyForEvents( bool isReady )
{
    LOG_SCOPE(areg_ipc_private_ClientSendThread_readyForEvents);

    if ( isReady )
    {
        LOG_DBG( "Starting client service dispatcher thread [ %s ]", getName( ).getString( ) );
        areg::SendMessageEvent::addListener( static_cast<areg::SendMessageEventConsumer &>(*this), static_cast<areg::DispatcherThread &>(*this) );
        areg::DispatcherThread::readyForEvents( true );
    }
    else
    {
        areg::DispatcherThread::readyForEvents( false );
        areg::SendMessageEvent::removeListener( static_cast<areg::SendMessageEventConsumer &>(*this), static_cast<areg::DispatcherThread &>(*this) );
        mConnection.closeSocket( );
        LOG_DBG( "Exiting client service dispatcher thread [ %s ], stopping receiving events", getName( ).getString( ) );
    }
}

void ClientSendThread::processEvent( const areg::SendMessageEventData & data )
{
    if ( data.isForwardMessage() )
    {
        const areg::RemoteMessage & msg = data.getRemoteMessage( );
        int32_t sizeSend = mConnection.sendMessage( msg );
        if ( sizeSend > 0 )
        {
            if (mSaveDataSend)
            {
                mBytesSend += static_cast<uint32_t>(sizeSend);
            }
        }
        else
        {
            mRemoteService.failedSendMessage( msg, mConnection.getSocket( ) );
        }
    }
    else if (data.isExitThreadMessage() )
    {
        mConnection.closeSocket( );
        triggerExit( );
    }
}

bool ClientSendThread::postEvent(areg::Event & eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, areg::SendMessageEvent) != nullptr) && areg::EventDispatcher::postEvent(eventElem);
}
