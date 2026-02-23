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
class AREG_API Socket
{
//////////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Default constructor. Creates instance of object
     *          with invalid socket object. Before sending
     *          or receiving data, the socket should be created.
     **/
    Socket();

    /**
     * \brief   Initialization constructor. Sets socket descriptor and socket address
     * \param   hSocket         Socket descriptor to set
     * \param   sockAddress     Socket address to set
     **/
    Socket( const SOCKETHANDLE hSocket, const areg::SocketAddress & sockAddress );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    Socket( const Socket & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source to copy data.
     **/
    Socket( Socket && source ) noexcept;

    /**
     * \brief   Destructor. Invalidates socket object, decrease reference counter,
     *          and if reference counter is reaching zero, close socket.
     **/
    virtual ~Socket();

    /**
     *  \brief	Assigns socket data taken from given source.
     *  \param	src		The source of socket data.
     **/
    Socket & operator = ( const Socket & src );

    /**
     *  \brief	Moves socket data taken from given source.
     *  \param	src		The source of socket data.
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
     * \brief   For client sockets, this method is creating new socket descriptor
     *          and connects to specified remote host and port number.
     *          For server sockets, this method is creating new socket descriptor
     *          and bind socket to specified host name and port number.
     *          The method should not do anything for accepted sockets.
     *          If object had before valid socket descriptor, it will be first closed,
     *          then create new.
     *
     *          The method should be overwritten by child classes.
     *
     * \param   hostName    The name of host to connect or bind.
     * \param   portNr      The valid port number to connect or bind.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool createSocket( const char * hostName, uint16_t portNr ) = 0;

    /**
     * \brief   For client sockets, this method is creating new socket descriptor
     *          and connects to host and port number. Both, remote host address
     *          and port number should be already set.
     *          For server sockets, this method is creating new socket descriptor
     *          and bind socket to specified host name and port number. 
     *          Both, socket IP-address and port number should be already set.
     *          The method should not do anything for accepted sockets.
     *          If object had before valid socket descriptor, it will be first closed,
     *          then create new.
     *
     *          The method should be overwritten by child classes.
     *
     * \return  Returns true if operation succeeded.
     **/
    virtual bool createSocket() = 0;

    /**
     * \brief   Closes existing socket.
     *          Note:   The call will invalidate socket of object, but the socket 
     *                  will be closed only if reference count to existing valid 
     *                  socket reaches zero.
     **/
    virtual void closeSocket();

    /**
     * \brief   If socket is valid, sends data using existing socket connection and returns
     *          number sent of bytes. And returns negative number if either socket is invalid,
     *          or failed to send data to remote host.
     *          Note:   The call is blocking and method will not return until all data are not sent
     *                  or if data sending fails.
     * \param   buffer  The buffer of data to send to remote target.
     * \param   length  The length in bytes of data in buffer to send
     * \return  Returns number of bytes sent to remote target. 
     *          Returns negative number if socket is not valid of failed to send.
     **/
    virtual int32_t sendData( const uint8_t * buffer, int32_t length ) const;

    /**
     * \brief   If socket is valid, receives data using existing socket connection and returns
     *          number of received bytes in buffer, which is equal to specified length parameter.
     *          Returns negative number if either socket is invalid, or failed to receive data from remote host.
     *          Note:   The call is blocking and method will not return until all data specified in length
     *                  is not received or if receiving data fails.
     * \param   buffer  The buffer to fill received data from remote target.
     * \param   length  The length in bytes of allocated space in buffer.
     * \return  Returns number of bytes received from remote target. 
     *          Returns negative number if socket is not valid of failed to receive data.
     **/
    virtual int32_t receiveData( uint8_t * buffer, int32_t length ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns socket descriptor of object.
     *          If socket was created, the value is not equal to areg::InvalidSocketHandle
     **/
    inline SOCKETHANDLE getHandle() const;

    /**
     * \brief   Returns true if existing socket descriptor is valid.
     *          The function is not checking socket descriptor validation.
     **/
    inline bool isValid() const;

    /**
     * \brief   Checks and returns socket alive state.
     * \return  Returns true if socket is alive and is not closed.
     **/
    inline bool isAlive() const;

    /**
     * \brief   Checks and returns number of bytes remain to read from socket buffer.
     *          Returns negative value if socket is invalid.
     * \return  Returns number of bytes available to read from socket buffer.
     **/
    inline int32_t pendingRead() const;

    /**
     * \brief   Sets socket in read-only more, i.e. no send message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableSend() const;

    /**
     * \brief   Sets socket in write-only more, i.e. no receive message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableReceive() const;

    /**
     * \brief   Return Socket Address object.
     **/
    inline const areg::SocketAddress & getAddress() const;

    /**
     * \brief   Sets socket address. The address should be either invalid
     *          or already resolved with IP-address.
     * \param   newAddress  The new address to set.
     **/
    inline void setAddress( const areg::SocketAddress & newAddress );

    /**
     * \brief   Sets Socket Address. If hostName is not IP-address, it will 
     *          try to resolve first then set. The isServer parameter is needed
     *          to resolve address either for server or for client.
     *          For accepted sockets this call plays no role, because the
     *          the address automatically is resolved when accepting connection.
     * \param   hostName    Host name or IP-address to set. If name is specified,
     *                      first it will be resolved to get IP-address.
     * \param   portNr      Valid port number of socket connection.
     * \param   isServer    Flag, indicating whether name should be resolve for
     *                      server or for client.
     * \return  Returns true if succeeded to resolve and set Socket Address.
     **/
    bool setAddress( const char * hostName, uint16_t portNr, bool isServer );

    /**
     * \brief   Returns the packet size in bytes to send data.
     **/
    inline uint32_t getSendPacketSize() const;

    /**
     * \brief   Returns the packet size in bytes to receive data.
     **/
    inline uint32_t getRecvPacketSize() const;

protected:
/************************************************************************/
// Socket protected overrides
/************************************************************************/

    /**
     * \brief   Called when the lock counter reaches zero.
     *          By default, when lock counter is zero, the system automatically closes socket.
     *          Overwrite this method if other action should be taken.
     * \param   hSocket     The Socket Handle to take close action.
     *                      In the moment when it is called, the member socket handle is already invalidated.
     **/
	void closeSocketHandle( SOCKETHANDLE hSocket );

    /**
     * \brief   Decreases lock counter and if it is zero, the calls method to close socket.
     **/
    void decreaseLock();

    /**
     * \brief   Sets the socket packet size in bytes to send data. The packet cannot be smaller than areg::PACKET_MIN_SIZE
     *          and bigger than areg::PACKET_MAX_SIZE. 
     * 
     * \param   sendSize    The size of packet in bytes to set to send data.
     *                      The function checks and normalizes size in range between 
     *                      areg::PACKET_MIN_SIZE and areg::PACKET_MAX_SIZE.
     * \param   force       If true, it forces to update the packet size. Otherwise, the packet size
     *                      is updated only if new size is bigger than the actual.
     * \return  Returns the actual size of packet in bytes to send data. If socket is not valid,
     *          return areg::PACKET_INVALID_SIZE.
     **/
    uint32_t setSendPacketSize(uint32_t sendSize, bool force = false) const;

     /**
      * \brief   Sets the socket packet size in bytes to receive data. The packet cannot be smaller than areg::PACKET_MIN_SIZE
      *          and bigger than areg::PACKET_MAX_SIZE.
      *
      * \param   recvSize    The size of packet in bytes to set to receive data.
      *                      The function checks and normalizes size in range between
      *                      areg::PACKET_MIN_SIZE and areg::PACKET_MAX_SIZE.
      * \param   force       If true, it forces to update the packet size. Otherwise, the packet size
      *                      is update only if new size is bigger than the actual.
      * \return  Returns the actual size of packet in bytes to receive data. If socket is not valid,
      *          return areg::PACKET_INVALID_SIZE.
      **/
    uint32_t setRecvPacketSize(uint32_t recvSize, bool force = false) const;

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

inline SOCKETHANDLE Socket::getHandle() const
{
    return (mSocket.get() != nullptr ? *mSocket : areg::InvalidSocketHandle);
}

inline const areg::SocketAddress & Socket::getAddress() const
{
    return mAddress;
}

inline void Socket::setAddress( const areg::SocketAddress & newAddress )
{
    mAddress = newAddress;
}

inline bool Socket::isValid() const
{
    return (mSocket.get() != nullptr) && areg::isSocketHandleValid(*mSocket);
}

inline bool Socket::isAlive() const
{
    return (mSocket.get() != nullptr) && areg::isSocketAlive(*mSocket);
}

inline int32_t Socket::pendingRead() const
{
    return (mSocket.get() != nullptr) && areg::pendingRead(*mSocket);
}

inline bool Socket::disableSend() const
{
    return (mSocket.get() != nullptr) && areg::disableSend(*mSocket);
}

inline bool Socket::disableReceive() const
{
    return (mSocket.get() != nullptr) && areg::disableReceive(*mSocket);
}

inline uint32_t Socket::getSendPacketSize() const
{
    return (isValid() ? mSendSize : areg::PACKET_INVALID_SIZE);
}

uint32_t Socket::getRecvPacketSize() const
{
    return (isValid() ? mRecvSize : areg::PACKET_INVALID_SIZE);
}

#endif  // AREG_BASE_SOCKET_HPP
