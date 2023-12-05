/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/service/private/ServerConnection.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, service server connection class.
 ************************************************************************/
#include "extend/service/ServerConnection.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/component/NEService.hpp"
#include "areg/base/RemoteMessage.hpp"

ServerConnection::ServerConnection(const ITEM_ID & channelId )
    : ServerConnectionBase  ( )
    , SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

ServerConnection::ServerConnection(const ITEM_ID & channelId, const char * hostName, unsigned short portNr)
    : ServerConnectionBase  ( hostName, portNr)
    , SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

ServerConnection::ServerConnection(const ITEM_ID & channelId, const NESocket::SocketAddress & serverAddress)
    : ServerConnectionBase  ( serverAddress )
    , SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

void ServerConnection::rejectConnection(SocketAccepted & clientConnection)
{
    const ITEM_ID & cookie = getCookie(clientConnection.getHandle());
    RemoteMessage msgReject = NERemoteService::createRejectNotify(mChannelId, cookie);
    sendMessage(msgReject, clientConnection);
    closeConnection(clientConnection);
}

void ServerConnection::closeAllConnections(void)
{
    Lock lock( mLock );
    RemoteMessage msgByeClient;
    if ( msgByeClient.initMessage(NERemoteService::getMessageNotifyClientConnection().rbHeader ) != nullptr )
    {
        msgByeClient.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );
        msgByeClient.setSource( mChannelId );

        for (MapSocketToObject::MAPPOS pos = mAcceptedConnections.firstPosition(); mAcceptedConnections.isValidPosition(pos); pos = mAcceptedConnections.nextPosition(pos))
        {
            SocketAccepted clientConnection = mAcceptedConnections.valueAtPosition(pos);
            const ITEM_ID& target{ getCookie(clientConnection) };
            if (target >= NEService::COOKIE_REMOTE_SERVICE)
            {
                RemoteMessage msgDisconnect{ msgByeClient.clone() };
                msgDisconnect.setTarget(target);
                msgDisconnect << target << NEService::eServiceConnection::ServiceDisconnected;
                sendMessage(msgDisconnect, clientConnection);
            }
        }
    }

    mMasterList.clear();
    mCookieToSocket.clear();
    mSocketToCookie.clear();
    mAcceptedConnections.clear();

    mCookieGenerator    = NEService::COOKIE_REMOTE_SERVICE;
}
