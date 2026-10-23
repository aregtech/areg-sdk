/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/ServerConnection.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, service server connection class.
 ************************************************************************/
#include "aregextend/service/ServerConnection.hpp"

#include "aregextend/service/SystemServiceDefs.hpp"
#include "aregextend/service/private/SocketHandover.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/base/MessageEnvelope.hpp"

#include <shared_mutex>
#include <vector>

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
    areg::MessageEnvelope msgReject{ areg::create_reject_notify(mChannelId, cookie_id) };
    send_message(msgReject, clientConnection);
    close_connection(clientConnection);
}

bool ServerConnection::accept_connection(SocketAccepted & clientConnection, SocketRelease & serving)
{
    if ( ServerConnectionBase::accept_connection(clientConnection) == false )
        return false;

    std::unique_lock<std::shared_mutex> lock( mLock );
    if ( mAcceptedConnections.contains(clientConnection.handle()) == false )
        return false;

    mServing.set_value_at(clientConnection.handle(), &serving);
    return true;
}

void ServerConnection::close_connection(const SocketAccepted & clientConnection)
{
    ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
    SocketRelease * serving{ nullptr };
    SocketAccepted client;
    {
        std::unique_lock<std::shared_mutex> lock( mLock );
        client = _detach_nolock(clientConnection.handle(), cookie, serving);
    }

    if ( (serving != nullptr) && client.is_valid() )
    {
        serving->release_socket(cookie, std::move(client));
    }
}

void ServerConnection::close_connection(const ITEM_ID & cookie)
{
    ITEM_ID detached{ areg::COOKIE_UNKNOWN };
    SocketRelease * serving{ nullptr };
    SocketAccepted client;
    {
        std::unique_lock<std::shared_mutex> lock( mLock );
        const SOCKETHANDLE hSocket{ handle_by_cookie_nolock(cookie) };
        if ( areg::is_valid_socket(hSocket) )
        {
            client = _detach_nolock(hSocket, detached, serving);
        }
    }

    if ( (serving != nullptr) && client.is_valid() )
    {
        serving->release_socket(detached, std::move(client));
    }
}

void ServerConnection::close_all_connections()
{
    static constexpr areg::EventHeader HDR{ areg::notify_client_connection() };
    struct Closed
    {
        ITEM_ID         cookie  { areg::COOKIE_UNKNOWN };
        SocketRelease * serving { nullptr };
        SocketAccepted  socket  { };
    };

    std::vector<Closed> closed;
    {
        std::unique_lock<std::shared_mutex> lock( mLock );
        areg::MessageEnvelope msgByeClient;
        if ( msgByeClient.init_envelope(HDR, 2 * sizeof(ITEM_ID)) != nullptr )
        {
            msgByeClient.set_sequence( areg::SEQUENCE_NUMBER_ANY );
            msgByeClient.set_source( static_cast<uint32_t>(mChannelId) );

            for (MapSocketToObject::MAPPOS pos = mAcceptedConnections.first_position(); mAcceptedConnections.is_valid_position(pos); pos = mAcceptedConnections.next_position(pos))
            {
                SocketAccepted clientConnection = mAcceptedConnections.value_at(pos);
                // Read handle directly to avoid re-acquiring lock.
                const SOCKETHANDLE hSocket{ clientConnection.handle() };
                MapSocketToCookie::MAPPOS posC{ mSocketToCookie.find(hSocket) };
                const ITEM_ID target{ mSocketToCookie.is_valid_position(posC) ? mSocketToCookie.value_at(posC) : areg::COOKIE_UNKNOWN };
                if (target >= areg::COOKIE_REMOTE_SERVICE)
                {
                    areg::MessageEnvelope msgDisconnect{ msgByeClient.clone() };
                    msgDisconnect.set_target(static_cast<uint32_t>(target));
                    msgDisconnect << target << areg::ServiceConnectionState::Disconnected;
                    send_message(msgDisconnect, clientConnection);
                }
            }
        }

        closed.reserve(mAcceptedConnections.size());
        while ( mAcceptedConnections.is_empty() == false )
        {
            Closed entry{ };
            const SOCKETHANDLE hSocket{ mAcceptedConnections.value_at(mAcceptedConnections.first_position()).handle() };
            entry.socket = _detach_nolock(hSocket, entry.cookie, entry.serving);
            closed.push_back(std::move(entry));
        }

        mServing.clear();
        mCookieGenerator = areg::COOKIE_REMOTE_SERVICE;
    }

    for ( Closed & entry : closed )
    {
        if ( entry.serving != nullptr )
        {
            entry.serving->release_socket(entry.cookie, std::move(entry.socket));
        }
    }
}

void ServerConnection::close_socket()
{
    {
        std::unique_lock<std::shared_mutex> lock( mLock );
        mServing.clear();
    }

    ServerConnectionBase::close_socket();
}

void ServerConnection::batch_handles_by_cookies( const ITEM_ID * cookies
                                               , SOCKETHANDLE * handles
                                               , uint32_t count
                                               , HeldSocket * holds
                                               , uint32_t & holdCount ) const noexcept
{
    holdCount = 0u;
    std::shared_lock<std::shared_mutex> lock( mLock );
    for (uint32_t i = 0u; i < count; ++i)
    {
        MapCookieToSocket::MAPPOS pos = mCookieToSocket.find(cookies[i]);
        if ( mCookieToSocket.is_valid_position(pos) )
        {
            const CookieSocket & socket{ mCookieToSocket.value_at(pos) };
            handles[i] = socket.socket;
            areg::ext::hold_socket(socket.socket, socket.hold, holds, holdCount);
        }
        else
        {
            handles[i] = areg::InvalidSocketHandle;
        }
    }
}

SocketAccepted ServerConnection::_detach_nolock(SOCKETHANDLE hSocket, ITEM_ID & cookie, SocketRelease * & serving)
{
    serving = nullptr;
    SocketAccepted result{ detach_connection_nolock(hSocket, cookie) };
    if ( result.is_valid() )
    {
        MapSocketToRelease::MAPPOS pos{ mServing.find(hSocket) };
        if ( mServing.is_valid_position(pos) )
        {
            serving = mServing.value_at(pos);
            mServing.remove_at(hSocket);
        }
    }

    return result;
}

} // namespace areg::ext
