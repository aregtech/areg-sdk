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
    const ITEM_ID & cookie() const noexcept;

    /**
     * \brief   Sets the connection cookie identifier.
     **/
    void set_cookie(const ITEM_ID & newCookie ) noexcept;

    /**
     * \brief   Returns the socket address object.
     **/
    [[nodiscard]]
    const areg::SocketAddress & address() const noexcept;

    /**
     * \brief   Resolves host name and sets socket address.
     *
     * \param   hostName    Host name or IP-address to set. If name is specified, first it will be
     *                      resolved to get IP-address.
     * \param   portNr      Valid port number of socket connection.
     * \return  Returns true if host name resolution and address setting succeeded.
     **/
    bool set_address( const String & hostName, uint16_t portNr );

    /**
     * \brief   Sets socket address from a resolved SocketAddress object.
     *
     * \param   newAddress      The new address to set.
     **/
    void set_address( const areg::SocketAddress & newAddress );

    /**
     * \brief   Returns true if socket descriptor is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Returns reference to the underlying socket object.
     **/
    Socket & socket();

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
     * \brief   Sends message data via socket connection. Validates checksum before sending. Returns
     *          bytes sent, zero if invalid checksum, or negative on failure.
     *
     * \param   in_message      The instance of buffer to send. The checksum number of Remote Buffer
     *                          object will be checked before sending. If checksum is invalid, the
     *                          data will not be sent.
     * \return  Returns length in bytes of data sent; zero if checksum invalid or buffer empty;
     *          negative if socket invalid or send failed.
     **/
    int32_t send_message( const RemoteMessage & in_message ) const;

    /**
     * \brief   Receives message data via socket connection. Validates checksum after receiving.
     *          Returns bytes received, zero if invalid checksum, or negative on failure.
     *
     * \param[in,out] out_message     The instance of Remote Buffer to receive data. The checksum
     *                                number of Remote Buffer object will be checked after receiving
     *                                data. If checksum is invalid, the data will invalidated and
     *                                dropped.
     * \return  Returns length in bytes of data received; zero if checksum invalid or buffer empty;
     *          negative if socket invalid or receive failed.
     **/
    int32_t receive_message( RemoteMessage & out_message ) const;

    /**
     * \brief   Sets socket to read-only mode, disabling message sending.
     *
     * \return  Returns true if operation succeeds.
     **/
    bool disable_send();

    /**
     * \brief   Sets socket to write-only mode, disabling message receiving.
     *
     * \return  Returns true if operation succeeds.
     **/
    bool disable_receive();

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

inline int32_t ClientConnection::send_message(const RemoteMessage & in_message) const
{
    return SocketConnectionBase::send_message(in_message, mClientSocket);
}

inline int32_t ClientConnection::receive_message(RemoteMessage & out_message) const
{
    return SocketConnectionBase::receive_message(out_message, mClientSocket);
}

} // namespace areg
#endif  // AREG_IPC_CLIENTCONNECTION_HPP
