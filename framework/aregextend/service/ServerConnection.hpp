#ifndef AREG_AREGEXTEND_SERVICE_SERVERCONNECTION_HPP
#define AREG_AREGEXTEND_SERVICE_SERVERCONNECTION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/ServerConnection.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, service server connection class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/ipc/ServerConnectionBase.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"

#include "areg/base/Containers.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/SyncPrimitives.hpp"

namespace areg {
    class SocketMultiplexer;
}

namespace areg::ext {
    class SocketRelease;
    struct HeldSocket;
}

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// ServerConnection class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Server Socket is used to accept connection from remote clients,
 *          send and receive data. When connection from client is accepted,
 *          the server specifies unique cookie for accepted client. After
 *          connection is accepted, the client starts to send and receive data.
 *          Sending and receiving data are running in blocking mode. 
 *          To synchronize communication, there are send and receive threads
 *          are specified.
 **/
class ServerConnection  : public    ServerConnectionBase
                        , private   SocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The owner that serves each accepted socket, where the keys are socket handles.
     **/
    using MapSocketToRelease    = HashMap<SOCKETHANDLE, SocketRelease *>;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates instance of object with invalid socket object and sets the unique channel ID
     *          of the service in the system. Before sending or receiving data, the socket should be
     *          created and bound to socket address.
     *
     * \param   channelId       The unique channel ID of the service connectivity in the system.
     **/
    explicit ServerConnection(const ITEM_ID & channelId );

    /**
     * \brief   Creates instance of object with invalid socket object. Before sending or receiving
     *          data, the socket should be created and bound to specified local IP-address and port.
     *          When instantiated, it will resolved passed host name and port number. If succeeded
     *          to resolve, it will set resolved IP-address and port number as socket address. If
     *          passed hostName is nullptr, it resolve connection for local host.
     *
     * \param   channelId       The unique channel ID of the service connectivity in the system.
     * \param   hostName        Host name or IP-address of server.
     * \param   portNr          Port number of server.
     **/
    ServerConnection(const ITEM_ID & channelId, const char * hostName, uint16_t portNr );

    /**
     * \brief   Creates instance of object with invalid socket object. Before sending or receiving
     *          data, the socket should be created and bound to host and port. Specified
     *          serverAddress will be set as server address.
     *
     * \param   channelId           The unique channel ID of the service connectivity in the system.
     * \param   serverAddress       Address of server.
     **/
    ServerConnection(const ITEM_ID & channelId, const areg::SocketAddress & serverAddress );

    virtual ~ServerConnection() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the unique service connectivity channel ID in the system.
     **/
    inline const ITEM_ID & channel_id() const;

    /**
     * \brief   Call to reject connection. When rejected, the socket connection will be closed and
     *          no more data will be accepted from connection.
     *
     * \param   clientConnection    Connection to reject.
     **/
    void reject_connection( SocketAccepted & clientConnection );

    /**
     * \brief   Accepts the connection and records the owner that serves its socket.
     *
     * \param   clientConnection    The connection to accept.
     * \param   serving             The owner that serves the socket and takes it back on close.
     * \return  Returns true if the connection was accepted.
     **/
    bool accept_connection( SocketAccepted & clientConnection, SocketRelease & serving );

    /**
     * \brief   Moves an accepted socket to another serving owner and calls \a onMoved under the
     *          lock of the connection, so no close can come between the two.
     *
     * \param   clientConnection    The accepted connection.
     * \param   serving             The new owner that serves the socket.
     * \param   onMoved             Callable() that gives the socket to the new owner.
     * \return  Returns false if the connection is closed already; \a onMoved is not called then.
     **/
    template<typename MovedFn>
    bool move_serving( const SocketAccepted & clientConnection, SocketRelease & serving, MovedFn && onMoved );

    /**
     * \brief   Closes the connection: removes it from the maps, shuts its socket down and gives
     *          the socket back to the owner that serves it. Callable from any thread. The socket
     *          is closed when the last thread that uses it lets it go.
     *
     * \param   clientConnection    The connection to close.
     **/
    void close_connection( const SocketAccepted & clientConnection );

    /**
     * \brief   Closes the connection of the cookie, as close_connection() of the socket.
     *
     * \param   cookie      The cookie of the connection.
     **/
    void close_connection( const ITEM_ID & cookie );

    /**
     * \brief   Call to close all existing accepted connections.
     **/
    void close_all_connections();

    /**
     * \brief   Closes the server socket and forgets every accepted connection.
     **/
    void close_socket();

    /**
     * \brief   Resolves \a count cookie values to socket handles in a single shared-lock window,
     *          and keeps one reference to each socket found in \a holds. Invalid (disconnected)
     *          cookies are written as areg::InvalidSocketHandle.
     *
     * \param   cookies     Array of \a count cookie values to look up.
     * \param   handles     Output array; must hold at least \a count elements.
     * \param   count       Number of entries to resolve.
     * \param   holds       Output array of the distinct sockets found; at least \a count elements.
     * \param   holdCount   On output, the number of entries of \a holds.
     **/
    void batch_handles_by_cookies( const ITEM_ID * cookies
                                 , SOCKETHANDLE * handles
                                 , uint32_t count
                                 , HeldSocket * holds
                                 , uint32_t & holdCount ) const noexcept;

    /**
     * \brief   Returns the multiplexer the shared receive thread waits on.
     **/
    [[nodiscard]]
    inline areg::SocketMultiplexer & multiplexer() noexcept;

    /**
     * \brief   Sends an MessageEnvelope to an accepted client socket via scatter/gather I/O.
     *
     * \param   in_message      The envelope to send.
     * \param   clientSocket    The accepted socket object.
     * \return  Bytes sent on success; zero if nothing was sent; negative on error.
     **/
    inline int32_t send_message( const MessageEnvelope & in_message, const SocketAccepted & clientSocket ) const;

    /**
     * \brief   Sends an MessageEnvelope directly using a raw socket handle.
     *
     * \param   in_message  The envelope to send.
     * \param   hSocket     Raw OS socket handle of the target client.
     * \return  Bytes sent on success; zero if nothing was sent; negative on error.
     **/
    inline int32_t send_message( const MessageEnvelope & in_message, SOCKETHANDLE hSocket ) const;

    inline int32_t send_messages_batch(const areg::IoBuffer* messages, uint32_t count, const SocketAccepted& clientSocket, uint32_t totalSize = 0) const;

    inline int32_t send_messages_batch(const areg::IoBuffer* messages, uint32_t count, SOCKETHANDLE hSocket, uint32_t totalSize = 0) const;

    /**
     * \brief   Receives message data via socket connection into an MessageEnvelope.
     *          Returns bytes received, zero if the data is too long or incomplete, or negative on failure.
     *
     * \param[in,out] out_message     MessageEnvelope to receive into.
     * \param   clientSocket          The accepted socket object.
     * \return  Returns length in bytes of data received; zero if the data is too long or incomplete; negative on failure.
     **/
    inline int32_t receive_message( MessageEnvelope & out_message, const SocketAccepted & clientSocket ) const;

    /**
     * \brief   Removes the specified socket handle from the multiplexer watch set.
     *          After this call the ServerReceiveThread will no longer wake up for
     *          data on this socket. Call immediately after handing the socket off
     *          to a dedicated per-client receive thread.
     *
     * \param   hSocket     Handle of the socket to deregister.
     **/
    inline void unregister_from_multiplexer(SOCKETHANDLE hSocket) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Removes the connection from the maps under the held lock and returns its socket,
     *          its cookie and the owner that serves it.
     **/
    SocketAccepted _detach_nolock( SOCKETHANDLE hSocket, ITEM_ID & cookie, SocketRelease * & serving );

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The unique channel ID of the service connectivity in the system.
     **/
    const ITEM_ID               mChannelId;

    /**
     * \brief   The owner that serves each accepted socket. Guarded by the lock of the connection.
     **/
    MapSocketToRelease          mServing;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerConnection() = delete;
    AREG_NOCOPY_NOMOVE( ServerConnection );
};

//////////////////////////////////////////////////////////////////////////
// ServerConnection class inline functions
//////////////////////////////////////////////////////////////////////////

inline const ITEM_ID & ServerConnection::channel_id() const
{
    return mChannelId;
}

inline int32_t ServerConnection::send_message(const MessageEnvelope & in_message, const SocketAccepted & clientSocket) const
{
    return ServerConnection::send_message(in_message, clientSocket.handle());
}

inline int32_t ServerConnection::send_message(const MessageEnvelope & in_message, SOCKETHANDLE hSocket) const
{
    const areg::EventHeader* hdr{ in_message.header() };
    if (hdr == nullptr)
        return 0;

    const uint32_t wireSize{ static_cast<uint32_t>(sizeof(areg::EventHeader)) + hdr->bufHeader.biUsed };
    areg::IoBuffer ioBuffer{ reinterpret_cast<const uint8_t*>(hdr), wireSize };
    return SocketConnectionBase::send_messages_batch(&ioBuffer, 1u, hSocket, wireSize);
}

inline int32_t ServerConnection::send_messages_batch(const areg::IoBuffer* messages, uint32_t count, const SocketAccepted& clientSocket, uint32_t totalSize /*= 0*/) const
{
    return SocketConnectionBase::send_messages_batch(messages, count, clientSocket.handle(), totalSize);
}

inline int32_t ServerConnection::send_messages_batch(const areg::IoBuffer* messages, uint32_t count, SOCKETHANDLE hSocket, uint32_t totalSize /*= 0*/) const
{
    return SocketConnectionBase::send_messages_batch(messages, count, hSocket, totalSize);
}

inline int32_t ServerConnection::receive_message(MessageEnvelope & out_message, const SocketAccepted & clientSocket) const
{
    return SocketConnectionBase::receive_message(out_message, clientSocket);
}

inline void ServerConnection::unregister_from_multiplexer(SOCKETHANDLE hSocket) noexcept
{
    mMultiplexer.unregister_socket(hSocket);
}

inline areg::SocketMultiplexer & ServerConnection::multiplexer() noexcept
{
    return mMultiplexer;
}

template<typename MovedFn>
bool ServerConnection::move_serving( const SocketAccepted & clientConnection, SocketRelease & serving, MovedFn && onMoved )
{
    std::unique_lock<std::shared_mutex> lock( mLock );
    if ( mAcceptedConnections.contains(clientConnection.handle()) == false )
        return false;

    mServing.set_value_at(clientConnection.handle(), &serving);
    onMoved();
    return true;
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_SERVERCONNECTION_HPP
