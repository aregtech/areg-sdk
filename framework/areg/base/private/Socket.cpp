/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Socket.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform base class of socket.
 ************************************************************************/
#include "areg/base/Socket.hpp"

#include "areg/base/UtilityDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/RemoteMessage.hpp"

#include <atomic>
#include <utility>

//////////////////////////////////////////////////////////////////////////
// Socket class implementation
//////////////////////////////////////////////////////////////////////////

Socket::Socket()
    : mSocket   ( )
    , mAddress  ( )
    , mSendSize ( NESocket::PACKET_DEFAULT_SIZE)
    , mRecvSize ( NESocket::PACKET_DEFAULT_SIZE)
{
    static_cast<void>(NESocket::socket_initialize( ));
}

Socket::Socket(const SOCKETHANDLE hSocket, const NESocket::SocketAddress & sockAddress)
    : mSocket   ( std::make_shared<SOCKETHANDLE>(hSocket) )
    , mAddress  ( sockAddress )
    , mSendSize ( NESocket::PACKET_DEFAULT_SIZE )
    , mRecvSize ( NESocket::PACKET_DEFAULT_SIZE )
{
    static_cast<void>(NESocket::socket_initialize( ));
    mSendSize = send_packet_size();
    mRecvSize = recv_packet_size();
}

Socket::Socket( const Socket & source )
    : mSocket   ( source.mSocket )
    , mAddress  ( source.mAddress )
    , mSendSize ( source.mSendSize )
    , mRecvSize ( source.mRecvSize )
{
    static_cast<void>(NESocket::socket_initialize( ));
}

Socket::Socket( Socket && source ) noexcept
    : mSocket   ( std::move(source.mSocket)  )
    , mAddress  ( std::move(source.mAddress) )
    , mSendSize ( source.mSendSize )
    , mRecvSize ( source.mRecvSize )
{
    static_cast<void>(NESocket::socket_initialize( ));
}

Socket::~Socket()
{
    decrease_lock();
    static_cast<void>(NESocket::socket_release());
}

Socket & Socket::operator = ( const Socket & src )
{
	if ( this != &src )
	{
		decrease_lock();

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
		decrease_lock();

		this->mSocket 	= src.mSocket;
		this->mAddress	= std::move(src.mAddress);
        this->mSendSize = src.mSendSize;
        this->mRecvSize = src.mSendSize;
    }

	return (*this);
}

void Socket::close_socket()
{
    decrease_lock( );
}

int32_t Socket::send_data( const uint8_t * buffer, int32_t length ) const
{
    return (is_valid() ? NESocket::send_data( *mSocket, buffer, static_cast<uint32_t>(length), static_cast<uint32_t>(mSendSize) ) : -1);
}

int32_t Socket::receive_data( uint8_t * buffer, int32_t length ) const
{
    return (is_valid( ) ? NESocket::receive_data( *mSocket, buffer, static_cast<uint32_t>(length), static_cast<uint32_t>(mRecvSize) ) : -1);
}

bool Socket::set_address(const char * hostName, uint16_t portNr, bool isServer)
{
    if ( is_valid() && (mAddress.is_equal_address(hostName, portNr) == false))
    {
        ASSERT(mSocket.get() != nullptr);
        decrease_lock( );
    }

    return mAddress.resolve_address(hostName, portNr, isServer);
}

void Socket::decrease_lock()
{
    if ( is_valid() )
    {
        ASSERT( (mSocket.get( ) != nullptr) && (mSocket.use_count() != 0) );

        if ( mSocket.use_count() == 1 )
        {
            close_socket_handle( *mSocket );
        }

        mSocket.reset();
    }
}

void Socket::close_socket_handle( SOCKETHANDLE hSocket )
{
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        NESocket::socket_close(hSocket);
    }
}

uint32_t Socket::set_send_size(uint32_t sendSize, bool force /*= false*/) const
{
    if (is_valid() == false)
    {
        return NESocket::PACKET_INVALID_SIZE;
    }

    if (force || (sendSize > mSendSize))
    {
        mSendSize = NESocket::set_send_size(*mSocket, sendSize);
    }

    return mSendSize;
}

uint32_t Socket::set_recv_size(uint32_t recvSize, bool force /*= false*/) const
{
    if (is_valid() == false)
    {
        return NESocket::PACKET_INVALID_SIZE;
    }

    if (force || (recvSize > mRecvSize))
    {
        mRecvSize = NESocket::set_recv_size(*mSocket, recvSize);
    }

    return mRecvSize;
}
