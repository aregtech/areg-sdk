#ifndef AREG_IPC_CLIENTCONNECTION_HPP
#define AREG_IPC_CLIENTCONNECTION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ClientConnection.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Client Connection class declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/ipc/SocketConnectionBase.hpp"

#include "areg/base/EventEnvelope.hpp"
#include "areg/base/SocketClient.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TCP/IP client socket for connecting to remote host, sending and receiving data in
 *          blocking mode. Supports address resolution and connection state management.
 **/
class AREG_API ClientConnection : private   SocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ClientConnection();

    /**
     * \brief   Creates instance and resolves specified host name and port to socket address.
     *
     * \param   hostName    Host name or IP-address of remote server to connect. If nullptr,
     *                      resolves to localhost.
     * \param   portNr      Port number of remote server to connect, should not be invalid port.
     **/
    ClientConnection( const String & hostName, uint16_t portNr );

    /**
     * \brief   Creates instance with specified remote server socket address.
     *
     * \param   remoteAddress       Address of remote host to connect.
     **/
    ClientConnection( const areg::SocketAddress & remoteAddress );

    virtual ~ClientConnection() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the connection cookie identifier set by server.
     **/
    [[nodiscard]]
    inline const ITEM_ID & cookie() const noexcept;

    /**
     * \brief   Sets the connection cookie identifier.
     **/
    inline void set_cookie(const ITEM_ID & newCookie ) noexcept;

    /**
     * \brief   Returns the socket address object.
     **/
    [[nodiscard]]
    inline const areg::SocketAddress & address() const noexcept;

    /**
     * \brief   Resolves host name and sets socket address.
     *
     * \param   hostName    Host name or IP-address to set. If name is specified, first it will be
     *                      resolved to get IP-address.
     * \param   portNr      Valid port number of socket connection.
     * \return  Returns true if host name resolution and address setting succeeded.
     **/
    inline bool set_address( const String & hostName, uint16_t portNr );

    /**
     * \brief   Sets socket address from a resolved SocketAddress object.
     *
     * \param   newAddress      The new address to set.
     **/
    inline void set_address( const areg::SocketAddress & newAddress );

    /**
     * \brief   Returns true if socket descriptor is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns reference to the underlying socket object.
     **/
    [[nodiscard]]
    inline Socket & socket();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Creates socket descriptor and connects to specified remote host and port.
     *
     * \param   hostName    The name of host to connect.
     * \param   portNr      The valid port number to connect.
     * \return  Returns true if socket creation and connection succeeded.
     **/
    bool create_socket( const String & hostName, uint16_t portNr );

    /**
     * \brief   Creates socket descriptor and connects to existing socket address. Address must be
     *          set before calling.
     *
     * \return  Returns true if socket creation and connection succeeded.
     **/
    bool create_socket();

    /**
     * \brief   Closes the socket and disconnects from remote server.
     **/
    void close_socket();

public:
    /**
     * \brief   Sends a batch of messages via scatter-gather (writev / WSASend) in a single syscall.
     *
     * \param   messages    Array of pointers to messages to send. Entries may be nullptr (skipped).
     * \param   count       Number of entries in the array.
     * \return  Returns total bytes sent on success, or 0 / negative on failure.
     *
     * \note    Threading: call only from the send thread that owns this connection.
     **/
    inline int32_t send_messages_batch(const areg::IoBuffer* ioBuffer, uint32_t count, uint32_t totalSize = 0) const;

    /**
     * \brief   Receives message data via socket connection. Validates checksum after receiving.
     *          Returns bytes received, zero if invalid checksum, or negative on failure.
     *
     * \param[in,out] out_message     EventEnvelope to receive into; checksum validated after receiving.
     * \return  Returns length in bytes of data received; zero if checksum invalid or buffer empty;
     *          negative if socket invalid or receive failed.
     **/
    inline int32_t receive_message( EventEnvelope & out_message ) const;

    /**
     * \brief   Sends an EventEnvelope over the socket connection via scatter/gather I/O.
     *          Calls buffer_completion_fix() before sending to ensure checksum is computed.
     *
     * \param   env     The envelope to send.
     * \return  Returns bytes sent on success, or 0 / negative on failure.
     **/
    inline int32_t send_message( const EventEnvelope & env ) const;

    /**
     * \brief   Configures the socket-buffer sizes applied when create_socket() succeeds.
     *          Call before create_socket().  Values of zero leave the platform defaults.
     *          Windows applies only SO_SNDBUF and preserves OS receive-window autotuning.
     *
     * \param   sendBuf     Desired SO_SNDBUF size in bytes (0 = keep default).
     * \param   recvBuf     Desired SO_RCVBUF size in bytes (0 = keep default).
     **/
    inline void set_socket_buffers(uint32_t sendBuf, uint32_t recvBuf) noexcept;

    /**
     * \brief   Configures the SO_SNDTIMEO value applied when create_socket() succeeds.
     *          Call before create_socket().  A value of zero leaves the compile-time default.
     *
     * \param   timeoutMs   Desired SO_SNDTIMEO in milliseconds (0 = keep default).
     **/
    inline void set_send_timeout(uint32_t timeoutMs) noexcept;

    /**
     * \brief   Sets socket to read-only mode, disabling message sending.
     *
     * \return  Returns true if operation succeeds.
     **/
    inline bool disable_send();

    /**
     * \brief   Sets socket to write-only mode, disabling message receiving.
     *
     * \return  Returns true if operation succeeds.
     **/
    inline bool disable_receive();

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The client connection socket
     **/
    SocketClient    mClientSocket;

    /**
     * \brief   Client connection cookie
     **/
    ITEM_ID         mCookie;

    /**
     * \brief   SO_SNDBUF size applied after create_socket() succeeds.
     **/
    uint32_t        mSockSendBuf;

    /**
     * \brief   Requested SO_RCVBUF size for create_socket().
     **/
    uint32_t        mSockRecvBuf;

    /**
     * \brief   SO_SNDTIMEO value in ms applied after create_socket() succeeds.
     **/
    uint32_t        mSockSendTimeoutMs;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ClientConnection );
};

//////////////////////////////////////////////////////////////////////////
// ClientConnection class inline functions
//////////////////////////////////////////////////////////////////////////

inline const ITEM_ID & ClientConnection::cookie() const noexcept
{
    return mCookie;
}

inline void ClientConnection::set_cookie(const ITEM_ID & newCookie ) noexcept
{
    mCookie = newCookie;
}

inline const areg::SocketAddress & ClientConnection::address() const noexcept
{
    return mClientSocket.address();
}

inline bool ClientConnection::set_address(const String& hostName, uint16_t portNr)
{
    return mClientSocket.set_address(hostName, portNr, false);
}

inline void ClientConnection::set_address( const areg::SocketAddress & newAddress )
{
    mClientSocket.set_address(newAddress);
}

inline bool ClientConnection::is_valid() const noexcept
{
    return mClientSocket.is_valid();
}

inline bool ClientConnection::disable_send()
{
    return mClientSocket.disable_send( );
}

inline bool ClientConnection::disable_receive()
{
    return mClientSocket.disable_receive();
}

inline Socket & ClientConnection::socket()
{
    return mClientSocket;
}

inline int32_t ClientConnection::send_messages_batch(const areg::IoBuffer* ioBuffer, uint32_t count, uint32_t totalSize) const
{
    return SocketConnectionBase::send_messages_batch(ioBuffer, count, mClientSocket, totalSize);
}

inline int32_t ClientConnection::receive_message(EventEnvelope & out_message) const
{
    return SocketConnectionBase::receive_message(out_message, mClientSocket);
}

inline int32_t ClientConnection::send_message(const EventEnvelope & env) const
{
    const areg::EventHeader * hdr{ env.header() };
    if (hdr == nullptr)
        return 0;

    env.buffer_completion_fix();
    const uint32_t wireSize{ sizeof(areg::EventHeader) + hdr->bufHeader.biUsed };
    areg::IoBuffer ioBuffer{ reinterpret_cast<const uint8_t *>(hdr), wireSize };
    return send_messages_batch(&ioBuffer, 1u, wireSize);
}

inline void ClientConnection::set_socket_buffers(uint32_t sendBuf, uint32_t recvBuf) noexcept
{
    mSockSendBuf = (sendBuf > 0) ? sendBuf : mSockSendBuf;
    mSockRecvBuf = (recvBuf > 0) ? recvBuf : mSockRecvBuf;
}

inline void ClientConnection::set_send_timeout(uint32_t timeoutMs) noexcept
{
    mSockSendTimeoutMs = (timeoutMs > 0) ? timeoutMs : mSockSendTimeoutMs;
}

} // namespace areg
#endif  // AREG_IPC_CLIENTCONNECTION_HPP
