#ifndef AREG_BASE_SOCKET_HPP
#define AREG_BASE_SOCKET_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Socket.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform base class of socket.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SocketDefs.hpp"
#include <memory>

/************************************************************************
 * Dependencies
 ************************************************************************/
class RemoteMessage;

/**
 * \brief   Base class for client, server and accepted socket connections.
 *          The object cannot be directly instantiated. Instead, instantiate
 *          one of child classes.
 *          Current socket supports only TCP/IP connection. All other connection
 *          connection types and protocols are out of scope of this class and
 *          are not supported.
 *          The socket module will be automatically loaded in the process as
 *          soon as the first socket object is created and automatically released 
 *          when last socket is destroyed.
 **/
//////////////////////////////////////////////////////////////////////////
// Socket class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for client, server, and accepted socket connections. The object cannot be
 *          directly instantiated; instantiate a child class instead. Supports TCP/IP connections
 *          only.
 **/
class AREG_API Socket
{
//////////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Default constructor. Creates instance with invalid socket. Before sending or
     *          receiving data, the socket must be created.
     **/
    Socket();

    /**
     * \brief   Initializes the object with socket descriptor and socket address.
     *
     * \param   hSocket         Socket descriptor to set
     * \param   sockAddress     Socket address to set
     **/
    Socket( const SOCKETHANDLE hSocket, const NESocket::SocketAddress & sockAddress );

    /**
     * \brief   Copy constructor. Copies the socket state from source.
     *
     * \param   source      The source to copy data.
     **/
    Socket( const Socket & source );

    /**
     * \brief   Move constructor. Moves the socket state from source.
     *
     * \param   source      The source to move data.
     **/
    Socket( Socket && source ) noexcept;

    /**
     * \brief   Destructor. Invalidates socket object, decrease reference counter,
     *          and if reference counter is reaching zero, close socket.
     **/
    virtual ~Socket();

    /**
     * \brief   Copy assignment operator. Assigns socket data from given source.
     *
     * \param   src     The source of socket data.
     **/
    Socket & operator = ( const Socket & src );

    /**
     * \brief   Move assignment operator. Moves socket data from given source.
     *
     * \param   src     The source of socket data.
     **/
    Socket & operator = ( Socket && src ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Socket overrides
/************************************************************************/

    /**
     * \brief   Creates socket descriptor and connects (client) or binds (server) to specified host
     *          and port. Closes existing socket first if valid.
     *
     * \param   hostName    The name of host to connect or bind.
     * \param   portNr      The valid port number to connect or bind.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool create_socket( const char * hostName, uint16_t portNr ) = 0;

    /**
     * \brief   Creates socket descriptor and connects (client) or binds (server) using pre-set
     *          address and port. Closes existing socket first if valid.
     *
     * \return  Returns true if operation succeeded.
     **/
    virtual bool create_socket() = 0;

    /**
     * \brief   Closes existing socket. Note: The socket is closed only when reference count reaches
     *          zero.
     **/
    virtual void close_socket();

    /**
     * \brief   Sends data using existing socket connection. Returns number of bytes sent, or
     *          negative on failure. Call is blocking.
     *
     * \param   buffer      The buffer of data to send to remote target.
     * \param   length      The length in bytes of data in buffer to send
     * \return  Returns number of bytes sent to remote target. Returns negative number if socket is
     *          invalid or failed to send.
     **/
    virtual int32_t send_data( const uint8_t * buffer, int32_t length ) const;

    /**
     * \brief   Receives data using existing socket connection. Returns number of bytes received, or
     *          negative on failure. Call is blocking.
     *
     * \param[in,out] buffer      The buffer to fill with received data from remote target.
     * \param   length      The length in bytes of allocated space in buffer.
     * \return  Returns number of bytes received from remote target. Returns negative number if
     *          socket is invalid or failed to receive data.
     **/
    virtual int32_t receive_data( uint8_t * buffer, int32_t length ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns socket descriptor. If socket was created, the value is not equal to
     *          NESocket::InvalidSocketHandle.
     **/
    inline SOCKETHANDLE handle() const;

    /**
     * \brief   Returns true if socket descriptor is valid.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Returns true if socket is alive and not closed.
     *
     * \return  Returns true if socket is alive and is not closed.
     **/
    inline bool is_alive() const;

    /**
     * \brief   Returns number of bytes pending to read from socket buffer, or negative if socket is
     *          invalid.
     *
     * \return  Returns number of bytes available to read from socket buffer.
     **/
    inline int32_t pending_read() const;

    /**
     * \brief   Sets socket in read-only mode; no send is possible afterwards.
     *
     * \return  Returns true if operation succeeds.
     **/
    inline bool disable_send() const;

    /**
     * \brief   Sets socket in write-only mode; no receive is possible afterwards.
     *
     * \return  Returns true if operation succeeds.
     **/
    inline bool disable_receive() const;

    /**
     * \brief   Returns the socket address object.
     **/
    inline const NESocket::SocketAddress & address() const;

    /**
     * \brief   Sets socket address. Address should be invalid or pre-resolved with IP address.
     *
     * \param   newAddress      The new address to set.
     **/
    inline void set_address( const NESocket::SocketAddress & newAddress );

    /**
     * \brief   Sets socket address, resolving hostname to IP if needed. For accepted sockets,
     *          address is already resolved automatically.
     *
     * \param   hostName    Host name or IP-address to set. If name is specified, it will be
     *                      resolved to IP address.
     * \param   portNr      Valid port number of socket connection.
     * \param   isServer    Flag indicating whether name should be resolved for server or client.
     * \return  Returns true if succeeded to resolve and set Socket Address.
     **/
    bool set_address( const char * hostName, uint16_t portNr, bool isServer );

    /**
     * \brief   Returns the packet size in bytes for sending data.
     **/
    inline uint32_t send_packet_size() const;

    /**
     * \brief   Returns the packet size in bytes for receiving data.
     **/
    inline uint32_t recv_packet_size() const;

protected:
/************************************************************************/
// Socket protected overrides
/************************************************************************/

 /**
  * \brief   Called when lock counter reaches zero. By default, closes the socket. Override to
  *          perform other actions.
  *
  * \param   hSocket     The socket handle to close. The member socket handle is already invalidated
  *                      at this point.
  **/
	void close_socket_handle( SOCKETHANDLE hSocket );

    /**
     * \brief   Decreases lock counter and closes socket if counter reaches zero.
     **/
    void decrease_lock();

    /**
     * \brief   Sets the socket send packet size, constrained within NESocket::PACKET_MIN_SIZE and
     *          NESocket::PACKET_MAX_SIZE.
     *
     * \param   sendSize    The size of packet in bytes to set for sending. The function normalizes
     *                      size to valid range.
     * \param   force       If true, forces packet size update. Otherwise, updates only if new size
     *                      is bigger than actual.
     * \return  Returns the actual packet size in bytes for sending. Returns
     *          NESocket::PACKET_INVALID_SIZE if socket is invalid.
     **/
    uint32_t set_send_size(uint32_t sendSize, bool force = false) const;

    /**
     * \brief   Sets the socket receive packet size, constrained within NESocket::PACKET_MIN_SIZE
     *          and NESocket::PACKET_MAX_SIZE.
     *
     * \param   recvSize    The size of packet in bytes to set for receiving. The function
     *                      normalizes size to valid range.
     * \param   force       If true, forces packet size update. Otherwise, updates only if new size
     *                      is bigger than actual.
     * \return  Returns the actual packet size in bytes for receiving. Returns
     *          NESocket::PACKET_INVALID_SIZE if socket is invalid.
     **/
    uint32_t set_recv_size(uint32_t recvSize, bool force = false) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    
    /**
     * \brief   Pointer to socket descriptor. Also used as reference counter
     *          to close socket automatically if there is no more socket object holds the socket.
     **/
    std::shared_ptr<SOCKETHANDLE>   mSocket;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The address of socket
     **/
    NESocket::SocketAddress mAddress;

    /**
     * \brief   The size in bytes of packet to send data.
     *          Should be in range NESocket::PACKET_MIN_SIZE and NESocket::PACKET_MAX_SIZE.
     **/
    mutable uint32_t    mSendSize;

     /**
      * \brief   The size in bytes of packet to receive data.
      *          Should be in range NESocket::PACKET_MIN_SIZE and NESocket::PACKET_MAX_SIZE.
      **/
    mutable uint32_t    mRecvSize;
};

//////////////////////////////////////////////////////////////////////////
// Socket class inline functions
//////////////////////////////////////////////////////////////////////////

inline SOCKETHANDLE Socket::handle() const
{
    return (mSocket.get() != nullptr ? *mSocket : NESocket::InvalidSocketHandle);
}

inline const NESocket::SocketAddress & Socket::address() const
{
    return mAddress;
}

inline void Socket::set_address( const NESocket::SocketAddress & newAddress )
{
    mAddress = newAddress;
}

inline bool Socket::is_valid() const
{
    return (mSocket.get() != nullptr) && NESocket::is_handle_valid(*mSocket);
}

inline bool Socket::is_alive() const
{
    return (mSocket.get() != nullptr) && NESocket::is_socket_alive(*mSocket);
}

inline int32_t Socket::pending_read() const
{
    return (mSocket.get() != nullptr) && NESocket::pending_read(*mSocket);
}

inline bool Socket::disable_send() const
{
    return (mSocket.get() != nullptr) && NESocket::disable_send(*mSocket);
}

inline bool Socket::disable_receive() const
{
    return (mSocket.get() != nullptr) && NESocket::disable_receive(*mSocket);
}

inline uint32_t Socket::send_packet_size() const
{
    return (is_valid() ? mSendSize : NESocket::PACKET_INVALID_SIZE);
}

uint32_t Socket::recv_packet_size() const
{
    return (is_valid() ? mRecvSize : NESocket::PACKET_INVALID_SIZE);
}

#endif  // AREG_BASE_SOCKET_HPP
