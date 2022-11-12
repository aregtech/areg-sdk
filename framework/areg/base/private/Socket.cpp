/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Socket.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform base class of socket.
 ************************************************************************/
#include "areg/base/Socket.hpp"

#include "areg/base/NEUtilities.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/RemoteMessage.hpp"

#include <atomic>
#include <utility>

//////////////////////////////////////////////////////////////////////////
// Socket class implementation
//////////////////////////////////////////////////////////////////////////

Socket::Socket( void )
    : mSocket   ( )
    , mAddress  ( )
    , mSendSize ( NESocket::DEFAULT_SEGMENT_SIZE )
    , mRecvSize ( NESocket::DEFAULT_SEGMENT_SIZE )
{
    static_cast<void>(NESocket::socketInitialize( ));
}

Socket::Socket(const SOCKETHANDLE hSocket, const NESocket::SocketAddress & sockAddress)
    : mSocket   ( std::make_shared<SOCKETHANDLE>(hSocket) )
    , mAddress  ( sockAddress )
    , mSendSize ( NESocket::DEFAULT_SEGMENT_SIZE )
    , mRecvSize ( NESocket::DEFAULT_SEGMENT_SIZE )
{
    static_cast<void>(NESocket::socketInitialize( ));
}

Socket::Socket( const Socket & source )
    : mSocket   ( source.mSocket )
    , mAddress  ( source.mAddress )
    , mSendSize ( source.mSendSize )
    , mRecvSize ( source.mRecvSize )
{
    static_cast<void>(NESocket::socketInitialize( ));
}

Socket::Socket( Socket && source ) noexcept
    : mSocket   ( std::move(source.mSocket)  )
    , mAddress  ( std::move(source.mAddress) )
    , mSendSize ( source.mSendSize )
    , mRecvSize ( source.mRecvSize )
{
    static_cast<void>(NESocket::socketInitialize( ));
}

Socket::~Socket(void)
{
    decreaseLock();
    static_cast<void>(NESocket::socketRelease());
}

Socket & Socket::operator = ( const Socket & src )
{
	if ( this != &src )
	{
		decreaseLock();

		this->mSocket 	= src.mSocket;
		this->mAddress	= src.mAddress;
        this->mSendSize = src.mSendSize;
        this->mRecvSize = src.mSendSize;
	}

	return (*this);
}

Socket & Socket::operator = ( Socket && src ) noexcept
{
	if ( this != &src )
	{
		decreaseLock();

		this->mSocket 	= src.mSocket;
		this->mAddress	= std::move(src.mAddress);
        this->mSendSize = src.mSendSize;
        this->mRecvSize = src.mSendSize;
    }

	return (*this);
}

void Socket::closeSocket(void)
{
    decreaseLock( );
}

int Socket::sendData( const unsigned char * buffer, int length ) const
{
    int result = 0;
    if (isValid())
    {
        setSendSegmentSize(static_cast<unsigned int>(length), false);
        result = NESocket::sendData(*mSocket, buffer, length, mSendSize);
    }

    return result;
}

int Socket::receiveData( unsigned char * buffer, int length ) const
{
    int result = 0;
    if (isValid())
    {
        setRecvSegmentSize(static_cast<unsigned int>(length), false);
        result = NESocket::receiveData(*mSocket, buffer, length, mRecvSize);
    }

    return result;
}

bool Socket::setAddress(const char * hostName, unsigned short portNr, bool isServer)
{
    if ( isValid() && (mAddress.getHostAddress() != hostName || mAddress.getHostPort() != portNr) )
    {
        ASSERT(mSocket.get() != nullptr);
        decreaseLock( );
    }

    return mAddress.resolveAddress(hostName, portNr, isServer);
}

void Socket::decreaseLock(void)
{
    if ( isValid() )
    {
        ASSERT( (mSocket.get( ) != nullptr) && (mSocket.use_count() != 0) );

        if ( mSocket.use_count() == 1 )
        {
            closeSocketHandle( *mSocket );
        }

        mSocket.reset();
    }
}

void Socket::closeSocketHandle( SOCKETHANDLE hSocket )
{
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        NESocket::socketClose(hSocket);
    }
}

unsigned int Socket::remainRead( void ) const
{
    return (mSocket.get() != nullptr ? NESocket::remainDataRead(*mSocket) : 0);
}

unsigned int Socket::setSendSegmentSize(unsigned int sendSize, bool force /*= false*/) const
{
    if (isValid() == false)
    {
        return NESocket::SEGMENT_INVALID_SIZE;
    }

    if (force || (sendSize > mSendSize))
    {
        mSendSize = NESocket::setMaxSendSize(*mSocket, sendSize);
    }

    return mSendSize;
}

inline unsigned int Socket::setRecvSegmentSize(unsigned int recvSize, bool force /*= false*/) const
{
    if (isValid() == false)
    {
        return NESocket::SEGMENT_INVALID_SIZE;
    }

    if (force || (recvSize > mRecvSize))
    {
        mRecvSize = NESocket::setMaxSendSize(*mSocket, recvSize);
    }

    return mRecvSize;
}
