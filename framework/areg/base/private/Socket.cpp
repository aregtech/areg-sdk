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

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Socket class implementation
//////////////////////////////////////////////////////////////////////////

void Socket::_close_handle(SOCKETHANDLE h) noexcept
{
    if (areg::is_valid_socket(h))
        areg::socket_close(h);
}

Socket::Socket()
    : mSocket   ( )
    , mAddress  ( )
    , mSendSize ( areg::PACKET_DEFAULT_SIZE )
    , mRecvSize ( areg::PACKET_DEFAULT_SIZE )
{
    static_cast<void>(areg::socket_initialize( ));
}

Socket::Socket(const SOCKETHANDLE hSocket, const areg::SocketAddress & sockAddress)
    : mSocket   ( hSocket )
    , mAddress  ( sockAddress )
    , mSendSize ( areg::PACKET_DEFAULT_SIZE )
    , mRecvSize ( areg::PACKET_DEFAULT_SIZE )
{
    static_cast<void>(areg::socket_initialize( ));
    mSendSize = send_packet_size();
    mRecvSize = recv_packet_size();
}

Socket::Socket( const Socket & source )
    : mSocket   ( source.mSocket )      // SharedPrimitive copy: increments ref count
    , mAddress  ( source.mAddress )
    , mSendSize ( source.mSendSize )
    , mRecvSize ( source.mRecvSize )
{
    static_cast<void>(areg::socket_initialize( ));
}

Socket::Socket( Socket && source ) noexcept
    : mSocket   ( std::move(source.mSocket) )   // SharedPrimitive move: transfers ownership
    , mAddress  ( std::move(source.mAddress) )
    , mSendSize ( source.mSendSize )
    , mRecvSize ( source.mRecvSize )
{
    static_cast<void>(areg::socket_initialize( ));
}

Socket::~Socket()
{
    // mSocket: SharedPrimitive destructor releases the handle automatically
    static_cast<void>(areg::socket_release());
}

Socket & Socket::operator = ( const Socket & src )
{
    if ( this != &src )
    {
        mSocket   = src.mSocket;    // SharedPrimitive copy assign: releases old, shares new
        mAddress  = src.mAddress;
        mSendSize = src.mSendSize;
        mRecvSize = src.mRecvSize;
    }

    return (*this);
}

Socket & Socket::operator = ( Socket && src ) noexcept
{
    if ( this != &src )
    {
        mSocket   = std::move(src.mSocket); // SharedPrimitive move assign: releases old, takes ownership
        mAddress  = std::move(src.mAddress);
        mSendSize = src.mSendSize;
        mRecvSize = src.mRecvSize;
    }

    return (*this);
}

void Socket::close()
{
    decrease_lock( );
}

int32_t Socket::send( const uint8_t * buffer, int32_t length ) const
{
    return (is_valid() ? areg::send_data( mSocket.value(), buffer, static_cast<uint32_t>(length) ) : -1);
}

int32_t Socket::receive( uint8_t * buffer, int32_t length ) const
{
    return (is_valid() ? areg::receive_data( mSocket.value(), buffer, static_cast<uint32_t>(length) ) : -1);
}

bool Socket::set_address(const String& hostName, uint16_t portNr, bool isServer)
{
    if ( is_valid() && (mAddress.is_equal(hostName, portNr) == false))
    {
        decrease_lock( );
    }

    return mAddress.resolve_address(hostName, portNr, isServer);
}

void Socket::decrease_lock()
{
    mSocket.reset();
}

uint32_t Socket::set_send_size(uint32_t sendSize, bool force /*= false*/) const
{
    if (is_valid() == false)
    {
        return areg::PACKET_INVALID_SIZE;
    }

    if (force || (sendSize > mSendSize))
    {
        mSendSize = areg::set_send_size(mSocket.value(), sendSize);
    }

    return mSendSize;
}

uint32_t Socket::set_recv_size(uint32_t recvSize, bool force /*= false*/) const
{
    if (is_valid() == false)
    {
        return areg::PACKET_INVALID_SIZE;
    }

    if (force || (recvSize > mRecvSize))
    {
        mRecvSize = areg::set_recv_size(mSocket.value(), recvSize);
    }

    return mRecvSize;
}

} // namespace areg
