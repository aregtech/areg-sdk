/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServerConnection.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, service server connection class.
 ************************************************************************/
#include "aregextend/service/ServerConnection.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/base/RemoteMessage.hpp"

ServerConnection::ServerConnection(const ITEM_ID & channelId )
    : ServerConnectionBase  ( )
    , areg::SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

ServerConnection::ServerConnection(const ITEM_ID & channelId, const char * hostName, uint16_t portNr)
    : ServerConnectionBase  ( hostName, portNr)
    , areg::SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

ServerConnection::ServerConnection(const ITEM_ID & channelId, const areg::SocketAddress & serverAddress)
    : ServerConnectionBase  ( serverAddress )
    , areg::SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

void ServerConnection::rejectConnection(areg::SocketAccepted & clientConnection)
{
    const ITEM_ID & cookie = getCookie(clientConnection.getHandle());
    areg::RemoteMessage msgReject = areg::createRejectNotify(mChannelId, cookie);
    sendMessage(msgReject, clientConnection);
    closeConnection(clientConnection);
}

void ServerConnection::closeAllConnections()
{
    Lock lock( mLock );
    areg::RemoteMessage msgByeClient;
    if ( msgByeClient.initMessage(areg::getMessageNotifyClientConnection().rbHeader ) != nullptr )
    {
        msgByeClient.setSequenceNr( areg::SEQUENCE_NUMBER_ANY );
        msgByeClient.setSource( mChannelId );

        for (MapSocketToObject::MAPPOS pos = mAcceptedConnections.firstPosition(); mAcceptedConnections.isValidPosition(pos); pos = mAcceptedConnections.nextPosition(pos))
        {
            areg::SocketAccepted clientConnection = mAcceptedConnections.valueAtPosition(pos);
            const ITEM_ID& target{ getCookie(clientConnection) };
            if (target >= areg::COOKIE_REMOTE_SERVICE)
            {
                areg::RemoteMessage msgDisconnect{ msgByeClient.clone() };
                msgDisconnect.setTarget(target);
                msgDisconnect << target << areg::ServiceConnectionState::Disconnected;
                sendMessage(msgDisconnect, clientConnection);
            }
        }
    }

    mMasterList.clear();
    mCookieToSocket.clear();
    mSocketToCookie.clear();
    mAcceptedConnections.clear();

    mCookieGenerator    = areg::COOKIE_REMOTE_SERVICE;
}
