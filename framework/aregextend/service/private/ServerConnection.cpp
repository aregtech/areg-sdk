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

namespace areg::ext {

ServerConnection::ServerConnection(const ITEM_ID & channelId )
    : ServerConnectionBase  ( )
    , SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

ServerConnection::ServerConnection(const ITEM_ID & channelId, const char * hostName, uint16_t portNr)
    : ServerConnectionBase  ( hostName, portNr)
    , SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

ServerConnection::ServerConnection(const ITEM_ID & channelId, const areg::SocketAddress & serverAddress)
    : ServerConnectionBase  ( serverAddress )
    , SocketConnectionBase  ( )
    , mChannelId            ( channelId )
{
}

void ServerConnection::reject_connection(SocketAccepted & clientConnection)
{
    const ITEM_ID & cookie_id = cookie(clientConnection.handle());
    RemoteMessage msgReject = areg::create_reject_notify(mChannelId, cookie_id);
    send_message(msgReject, clientConnection);
    close_connection(clientConnection);
}

void ServerConnection::close_all_connections()
{
    Lock lock( mLock );
    RemoteMessage msgByeClient;
    if ( msgByeClient.init_message(areg::notify_client_connection().rbHeader ) != nullptr )
    {
        msgByeClient.set_sequence( areg::SEQUENCE_NUMBER_ANY );
        msgByeClient.set_source( mChannelId );

        for (MapSocketToObject::MAPPOS pos = mAcceptedConnections.first_position(); mAcceptedConnections.is_valid_position(pos); pos = mAcceptedConnections.next_position(pos))
        {
            SocketAccepted clientConnection = mAcceptedConnections.value_at(pos);
            const ITEM_ID& target{ cookie(clientConnection) };
            if (target >= areg::COOKIE_REMOTE_SERVICE)
            {
                RemoteMessage msgDisconnect{ msgByeClient.clone() };
                msgDisconnect.set_target(target);
                msgDisconnect << target << areg::ServiceConnectionState::Disconnected;
                send_message(msgDisconnect, clientConnection);
            }
        }
    }

    mMasterList.clear();
    mCookieToSocket.clear();
    mSocketToCookie.clear();
    mAcceptedConnections.clear();

    mCookieGenerator    = areg::COOKIE_REMOTE_SERVICE;
}

} // namespace areg::ext
