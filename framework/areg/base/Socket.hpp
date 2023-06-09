#ifndef AREG_BASE_SOCKET_HPP
#define AREG_BASE_SOCKET_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/Socket.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform base class of socket.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NESocket.hpp"
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
    Socket( void );

    /**
     * \brief   Initialization constructor. Sets socket descriptor and socket address
     * \param   hSocket         Socket descriptor to set
     * \param   sockAddress     Socket address to set
     **/
    Socket( const SOCKETHANDLE hSocket, const NESocket::SocketAddress & sockAddress );

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
    virtual ~Socket( void );

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
    virtual bool createSocket( const char * hostName, unsigned short portNr ) = 0;

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
    virtual bool createSocket( void ) = 0;

    /**
     * \brief   Closes existing socket.
     *          Note:   The call will invalidate socket of object, but the socket 
     *                  will be closed only if reference count to existing valid 
     *                  socket reaches zero.
     **/
    virtual void closeSocket( void );

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
    virtual int sendData( const unsigned char * buffer, int length ) const;

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
    virtual int receiveData( unsigned char * buffer, int length ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns socket descriptor of object.
     *          If socket was created, the value is not equal to NESocket::InvalidSocketHandle
     **/
    inline SOCKETHANDLE getHandle( void ) const;

    /**
     * \brief   Returns true if existing socket descriptor is valid.
     *          The function is not checking socket descriptor validation.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Checks and returns socket alive state.
     * \return  Returns true if socket is alive and is not closed.
     **/
    inline bool isAlive( void ) const;

    /**
     * \brief   Checks and returns number of bytes remain to read from socket buffer.
     *          Returns negative value if socket is invalid.
     * \return  Returns number of bytes available to read from socket buffer.
     **/
    inline int pendingRead( void ) const;

    /**
     * \brief   Sets socket in read-only more, i.e. no send message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableSend( void ) const;

    /**
     * \brief   Sets socket in write-only more, i.e. no receive message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableReceive( void ) const;

    /**
     * \brief   Return Socket Address object.
     **/
    inline const NESocket::SocketAddress & getAddress( void ) const;

    /**
     * \brief   Sets socket address. The address should be either invalid
     *          or already resolved with IP-address.
     * \param   newAddress  The new address to set.
     **/
    inline void setAddress( const NESocket::SocketAddress & newAddress );

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
    bool setAddress( const char * hostName, unsigned short portNr, bool isServer );

    /**
     * \brief   Returns the segment size in bytes to send data.
     **/
    inline unsigned int getSendSegmentSize(void) const;

    /**
     * \brief   Returns the segment size in bytes to receive data.
     **/
    inline unsigned int getRecvSegmentSize(void) const;

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
    void decreaseLock( void );

    /**
     * \brief   Sets the segment size in bytes of socket to send data. Before checking, the method checks
     *          that new size of segment is between NESocket::MIN_SEGMENT_SIZE and NESocket::MAX_SEGMENT_SIZE.
     *          If value is NESocket::SEGMENT_INVALID_SIZE, it will set the size NESocket::DEFAULT_SEGMENT_SIZE.
     *          It updates the segment size only if the new value is bigger than the actual or it 'force' value is true.
     * 
     * \param   sendSize    The new size of segment in bytes to set for sending data.
     *                      The function checks and normalizes value if it is not in the range
     *                      between NESocket::MIN_SEGMENT_SIZE and NESocket::MAX_SEGMENT_SIZE.
     * \param   force       If set true, it forces to update the segment size. Otherwise, the segment size
     *                      is update only if new size is bigger than the actual.
     * \return  Returns the actual size of the segment in bytes to send data.
     *          Returns NESocket::SEGMENT_INVALID_SIZE if socket is not valid.
     **/
    unsigned int setSendSegmentSize(unsigned int sendSize, bool force = false) const;

    /**
     * \brief   Sets the segment size in bytes of socket to receive data. Before checking, the method checks
     *          that new size of segment is between NESocket::MIN_SEGMENT_SIZE and NESocket::MAX_SEGMENT_SIZE.
     *          If value is NESocket::SEGMENT_INVALID_SIZE, it will set the size NESocket::DEFAULT_SEGMENT_SIZE.
     *          It updates the segment size only if the new value is bigger than the actual or it 'force' value is true.
     *
     * \param   recvSize    The new size of segment in bytes to set for receiving data.
     *                      The function checks and normalizes value if it is not in the range
     *                      between NESocket::MIN_SEGMENT_SIZE and NESocket::MAX_SEGMENT_SIZE.
     * \param   force       If set true, it forces to update the segment size. Otherwise, the segment size
     *                      is update only if new size is bigger than the actual.
     * \return  Returns the actual size of the segment in bytes to receive data.
     *          Returns NESocket::SEGMENT_INVALID_SIZE if socket is not valid.
     **/
    unsigned int setRecvSegmentSize(unsigned int recvSize, bool force = false) const;

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
     * \brief   The size in bytes of segment to send data.
     *          It should not be less than NESocket::MIN_SEGMENT_SIZE and
     *          more than NESocket::MAX_SEGMENT_SIZE.
     *          The default value is NESocket::DEFAULT_SEGMENT_SIZE
     **/
    mutable unsigned int    mSendSize;

    /**
     * \brief   The size in bytes of segment to receive data.
     *          It should not be less than NESocket::MIN_SEGMENT_SIZE and
     *          more than NESocket::MAX_SEGMENT_SIZE.
     *          The default value is NESocket::DEFAULT_SEGMENT_SIZE
     **/
    mutable unsigned int    mRecvSize;
};

//////////////////////////////////////////////////////////////////////////
// Socket class inline functions
//////////////////////////////////////////////////////////////////////////

inline SOCKETHANDLE Socket::getHandle( void ) const
{
    return (mSocket.get() != nullptr ? *mSocket : NESocket::InvalidSocketHandle);
}

inline const NESocket::SocketAddress & Socket::getAddress( void ) const
{
    return mAddress;
}

inline void Socket::setAddress( const NESocket::SocketAddress & newAddress )
{
    mAddress = newAddress;
}

inline bool Socket::isValid( void ) const
{
    return (mSocket.get() != nullptr) && (*mSocket != NESocket::InvalidSocketHandle);
}

inline bool Socket::isAlive(void) const
{
    return (mSocket.get() != nullptr) && NESocket::isSocketAlive(*mSocket);
}

inline int Socket::pendingRead(void) const
{
    return (mSocket.get() != nullptr) && NESocket::pendingRead(*mSocket);
}

inline bool Socket::disableSend( void ) const
{
    return (mSocket.get() != nullptr) && NESocket::disableSend(*mSocket);
}

inline bool Socket::disableReceive( void ) const
{
    return (mSocket.get() != nullptr) && NESocket::disableReceive(*mSocket);
}

inline unsigned int Socket::getSendSegmentSize(void) const
{
    return (isValid() ? mSendSize : NESocket::SEGMENT_INVALID_SIZE);
}

unsigned int Socket::getRecvSegmentSize(void) const
{
    return (isValid() ? mRecvSize : NESocket::SEGMENT_INVALID_SIZE);
}

#endif  // AREG_BASE_SOCKET_HPP
