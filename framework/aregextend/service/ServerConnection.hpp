#ifndef AREG_AREGEXTEND_SERVICE_SERVERCONNECTION_HPP
#define AREG_AREGEXTEND_SERVICE_SERVERCONNECTION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/ServerConnection.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, service server connection class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/EventEnvelope.hpp"
#include "areg/ipc/ServerConnectionBase.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"

#include "areg/base/Containers.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/SyncPrimitives.hpp"

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
     * \brief   Call to close all existing accepted connections.
     **/
    void close_all_connections();

    /**
     * \brief   Sends an EventEnvelope to an accepted client socket via scatter/gather I/O.
     *
     * \param   in_message      The envelope to send.
     * \param   clientSocket    The accepted socket object.
     * \return  Bytes sent on success; zero on invalid checksum; negative on error.
     **/
    inline int32_t send_message( const EventEnvelope & in_message, const SocketAccepted & clientSocket ) const;

    /**
     * \brief   Sends an EventEnvelope directly using a raw socket handle.
     *
     * \param   in_message  The envelope to send.
     * \param   hSocket     Raw OS socket handle of the target client.
     * \return  Bytes sent on success; zero on invalid checksum; negative on error.
     **/
    inline int32_t send_message( const EventEnvelope & in_message, SOCKETHANDLE hSocket ) const;

    inline int32_t send_messages_batch(const areg::IoBuffer* messages, uint32_t count, const SocketAccepted& clientSocket, uint32_t totalSize = 0) const;

    inline int32_t send_messages_batch(const areg::IoBuffer* messages, uint32_t count, SOCKETHANDLE hSocket, uint32_t totalSize = 0) const;

    /**
     * \brief   Receives message data via socket connection into an EventEnvelope.
     *          Returns bytes received, zero if invalid checksum, or negative on failure.
     *
     * \param[in,out] out_message     EventEnvelope to receive into.
     * \param   clientSocket          The accepted socket object.
     * \return  Returns length in bytes of data received; zero if checksum invalid; negative on failure.
     **/
    inline int32_t receive_message( EventEnvelope & out_message, const SocketAccepted & clientSocket ) const;

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
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The unique channel ID of the service connectivity in the system.
     **/
    const ITEM_ID               mChannelId;

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

inline int32_t ServerConnection::send_message(const EventEnvelope & in_message, const SocketAccepted & clientSocket) const
{
    return ServerConnection::send_message(in_message, clientSocket.handle());
}

inline int32_t ServerConnection::send_message(const EventEnvelope & in_message, SOCKETHANDLE hSocket) const
{
    const areg::EventHeader* hdr{ in_message.header() };
    if (hdr == nullptr)
        return 0;

    in_message.buffer_completion_fix();
    const uint32_t wireSize{ sizeof(areg::EventHeader) + hdr->bufHeader.biUsed };
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

inline int32_t ServerConnection::receive_message(EventEnvelope & out_message, const SocketAccepted & clientSocket) const
{
    return SocketConnectionBase::receive_message(out_message, clientSocket);
}

inline void ServerConnection::unregister_from_multiplexer(SOCKETHANDLE hSocket) noexcept
{
    mMultiplexer.unregister_socket(hSocket);
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_SERVERCONNECTION_HPP
