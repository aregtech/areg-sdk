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
#include "areg/base/areg_global.h"

#include "areg/base/String.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SharedPrimitive.hpp"
#include "areg/base/SocketDefs.hpp"
namespace areg {

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
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Releases the OS socket handle. Called by SharedPrimitive when the
     *          last copy of this socket is destroyed.
     **/
    static void _close_handle(SOCKETHANDLE h) noexcept;

protected:
    /**
     * \brief   Reference-counted SOCKETHANDLE. Stores the handle directly in the
     *          object; allocates only an atomic int32_t on the heap as the shared
     *          reference counter.
     **/
    using SocketHandle = SharedPrimitive<SOCKETHANDLE, areg::InvalidSocketHandle, _close_handle>;

//////////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected
//////////////////////////////////////////////////////////////////////////////
protected:

    Socket();

    /**
     * \brief   Initializes the object with socket descriptor and socket address.
     *
     * \param   hSocket         Socket descriptor to set
     * \param   sockAddress     Socket address to set
     **/
    Socket( const SOCKETHANDLE hSocket, const areg::SocketAddress & sockAddress );

    Socket( const Socket & source );

    Socket( Socket && source ) noexcept;

    virtual ~Socket();

    Socket & operator = ( const Socket & src );

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
    virtual bool create(const String& hostName, uint16_t portNr) = 0;

    /**
     * \brief   Creates socket descriptor and connects (client) or binds (server) to specified host
     *          and port. Closes existing socket first if valid.
     *
     * \param   hostName    The name of host to connect or bind.
     * \param   portNr      The valid port number to connect or bind.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool create(const char * hostName, uint16_t portNr ) = 0;

    /**
     * \brief   Creates socket descriptor and connects (client) or binds (server) using pre-set
     *          address and port. Closes existing socket first if valid.
     *
     * \return  Returns true if operation succeeded.
     **/
    virtual bool create() = 0;

    /**
     * \brief   Closes existing socket. Note: The socket is closed only when reference count reaches
     *          zero.
     **/
    virtual void close();

    /**
     * \brief   Sends data using existing socket connection. Returns number of bytes sent, or
     *          negative on failure. Call is blocking.
     *
     * \param   buffer      The buffer of data to send to remote target.
     * \param   length      The length in bytes of data in buffer to send
     * \return  Returns number of bytes sent to remote target. Returns negative number if socket is
     *          invalid or failed to send.
     **/
    virtual int32_t send(const uint8_t * buffer, int32_t length ) const;

    /**
     * \brief   Receives data using existing socket connection. Returns number of bytes received, or
     *          negative on failure. Call is blocking.
     *
     * \param[in,out] buffer      The buffer to fill with received data from remote target.
     * \param   length      The length in bytes of allocated space in buffer.
     * \return  Returns number of bytes received from remote target. Returns negative number if
     *          socket is invalid or failed to receive data.
     **/
    virtual int32_t receive(uint8_t * buffer, int32_t length ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns socket descriptor. If socket was created, the value is not equal to
     *          areg::InvalidSocketHandle.
     **/
    [[nodiscard]]
    inline SOCKETHANDLE handle() const noexcept;

    /**
     * \brief   Returns true if socket descriptor is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns true if socket is alive and not closed.
     *
     * \return  Returns true if socket is alive and is not closed.
     **/
    [[nodiscard]]
    inline bool is_alive() const noexcept;

    /**
     * \brief   Returns number of bytes pending to read from socket buffer,
     *          or -1 if the socket is invalid.
     *
     * \return  Byte count available to read, or -1 on invalid socket.
     **/
    [[nodiscard]]
    inline int32_t pending_read() const noexcept;

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
    [[nodiscard]]
    inline const areg::SocketAddress & address() const noexcept;

    /**
     * \brief   Sets socket address. Address should be invalid or pre-resolved with IP address.
     *
     * \param   newAddress      The new address to set.
     **/
    inline void set_address( const areg::SocketAddress & newAddress );

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
    bool set_address( const String& hostName, uint16_t portNr, bool isServer );

    /**
     * \brief   Returns the packet size in bytes for sending data.
     **/
    [[nodiscard]] inline uint32_t send_packet_size() const noexcept;

    /**
     * \brief   Returns the packet size in bytes for receiving data.
     **/
    [[nodiscard]] inline uint32_t recv_packet_size() const noexcept;

protected:
/************************************************************************/
// Socket protected overrides
/************************************************************************/

    /**
     * \brief   Releases this shared_ptr copy of the socket handle.  When the last copy is
     *          released, the custom deleter in the constructor closes the OS socket automatically.
     **/
    void decrease_lock();

    /**
     * \brief   Sets the socket send packet size, constrained within areg::PACKET_MIN_SIZE and
     *          areg::PACKET_MAX_SIZE.
     *
     * \param   sendSize    The size of packet in bytes to set for sending. The function normalizes
     *                      size to valid range.
     * \param   force       If true, forces packet size update. Otherwise, updates only if new size
     *                      is bigger than actual.
     * \return  Returns the actual packet size in bytes for sending. Returns
     *          areg::PACKET_INVALID_SIZE if socket is invalid.
     **/
    uint32_t set_send_size(uint32_t sendSize, bool force = false) const;

    /**
     * \brief   Sets the socket receive packet size, constrained within areg::PACKET_MIN_SIZE
     *          and areg::PACKET_MAX_SIZE.
     *
     * \param   recvSize    The size of packet in bytes to set for receiving. The function
     *                      normalizes size to valid range.
     * \param   force       If true, forces packet size update. Otherwise, updates only if new size
     *                      is bigger than actual.
     * \return  Returns the actual packet size in bytes for receiving. Returns
     *          areg::PACKET_INVALID_SIZE if socket is invalid.
     **/
    uint32_t set_recv_size(uint32_t recvSize, bool force = false) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Reference-counted socket handle. Stores the OS descriptor directly in the
     *          object (no heap allocation for the handle). A single atomic<int32_t> is
     *          allocated on the heap as the shared reference counter. _close_handle() is
     *          called automatically when the last copy is destroyed.
     **/
    SocketHandle        mSocket;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   The address of socket
     **/
    areg::SocketAddress mAddress;

    /**
     * \brief   The size in bytes of packet to send data.
     *          Should be in range areg::PACKET_MIN_SIZE and areg::PACKET_MAX_SIZE.
     **/
    mutable uint32_t    mSendSize;

     /**
      * \brief   The size in bytes of packet to receive data.
      *          Should be in range areg::PACKET_MIN_SIZE and areg::PACKET_MAX_SIZE.
      **/
    mutable uint32_t    mRecvSize;
};

//////////////////////////////////////////////////////////////////////////
// Socket class inline functions
//////////////////////////////////////////////////////////////////////////

inline SOCKETHANDLE Socket::handle() const noexcept
{
    return mSocket.value();
}

inline const areg::SocketAddress & Socket::address() const noexcept
{
    return mAddress;
}

inline void Socket::set_address( const areg::SocketAddress & newAddress )
{
    mAddress = newAddress;
}

inline bool Socket::is_valid() const noexcept
{
    return mSocket.has_value();
}

inline bool Socket::is_alive() const noexcept
{
    return areg::is_socket_alive(mSocket.value());
}

inline int32_t Socket::pending_read() const noexcept
{
    return (is_valid() ? static_cast<int32_t>(areg::pending_read(mSocket.value())) : -1);
}

inline bool Socket::disable_send() const
{
    return is_valid() && areg::disable_send(mSocket.value());
}

inline bool Socket::disable_receive() const
{
    return is_valid() && areg::disable_receive(mSocket.value());
}

inline uint32_t Socket::send_packet_size() const noexcept
{
    return (is_valid() ? mSendSize : areg::PACKET_INVALID_SIZE);
}

inline uint32_t Socket::recv_packet_size() const noexcept
{
    return (is_valid() ? mRecvSize : areg::PACKET_INVALID_SIZE);
}

} // namespace areg
#endif  // AREG_BASE_SOCKET_HPP
