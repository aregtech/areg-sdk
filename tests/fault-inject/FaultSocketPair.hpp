#ifndef AREG_TESTS_FAULT_INJECT_FAULTSOCKETPAIR_HPP
#define AREG_TESTS_FAULT_INJECT_FAULTSOCKETPAIR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/FaultSocketPair.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. Loopback sockets of the fault tests, opened with plain
 *              system calls on Linux and Windows.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/SocketDefs.hpp"

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif  // NOMINMAX
    #include <WinSock2.h>
#else   // defined(_WIN32)
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif  // defined(_WIN32)

#include <cstring>

namespace fault_test
{
#if defined(_WIN32)
    using NativeSocket  = SOCKET;
    using AddressLength = int;
    constexpr NativeSocket  NO_SOCKET{ INVALID_SOCKET };

    inline void close_native( NativeSocket sock )
    {
        ::closesocket( sock );
    }
#else   // defined(_WIN32)
    using NativeSocket  = int;
    using AddressLength = socklen_t;
    constexpr NativeSocket  NO_SOCKET{ -1 };

    inline void close_native( NativeSocket sock )
    {
        ::close( sock );
    }
#endif  // defined(_WIN32)

    //!< Opens a listening loopback socket on a port the system chooses. Returns NO_SOCKET when the system refuses.
    inline NativeSocket open_listener( sockaddr_in & addr, int backlog )
    {
        // Windows sockets need the socket library started; the count is never released by a test child.
        static const bool started{ areg::socket_initialize( ) };
        static_cast<void>(started);

        const NativeSocket listener{ ::socket( AF_INET, SOCK_STREAM, 0 ) };
        if ( listener == NO_SOCKET )
            return NO_SOCKET;

        addr = sockaddr_in{ };
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
        addr.sin_port        = 0;
        AddressLength len{ static_cast<AddressLength>(sizeof( addr )) };
        if ( ( ::bind( listener, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) != 0 )
          || ( ::listen( listener, backlog ) != 0 )
          || ( ::getsockname( listener, reinterpret_cast<sockaddr *>(&addr), &len ) != 0 ) )
        {
            close_native( listener );
            return NO_SOCKET;
        }

        return listener;
    }

    //!< A connected TCP loopback pair of plain sockets.
    struct LoopPair
    {
        SOCKETHANDLE    client  { areg::InvalidSocketHandle };
        SOCKETHANDLE    server  { areg::InvalidSocketHandle };

        //!< Opens the pair. Returns false when the system refuses.
        bool open( )
        {
            sockaddr_in addr{ };
            const NativeSocket listener{ open_listener( addr, 4 ) };
            if ( listener == NO_SOCKET )
                return false;

            bool result{ false };
            const NativeSocket sock{ ::socket( AF_INET, SOCK_STREAM, 0 ) };
            if ( sock != NO_SOCKET )
            {
                client = static_cast<SOCKETHANDLE>(sock);
                if ( ::connect( sock, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == 0 )
                {
                    const NativeSocket accepted{ ::accept( listener, nullptr, nullptr ) };
                    if ( accepted != NO_SOCKET )
                    {
                        server = static_cast<SOCKETHANDLE>(accepted);
                        result = true;
                    }
                }
            }

            close_native( listener );
            return result;
        }

        //!< Closes both ends.
        void close( )
        {
            if ( client != areg::InvalidSocketHandle )
                areg::socket_close( client );

            if ( server != areg::InvalidSocketHandle )
                areg::socket_close( server );

            client = areg::InvalidSocketHandle;
            server = areg::InvalidSocketHandle;
        }

        ~LoopPair( )
        {
            close( );
        }
    };

    //!< A listening loopback socket and its address.
    struct LoopListener
    {
        SOCKETHANDLE        handle  { areg::InvalidSocketHandle };
        areg::SocketAddress address { };

        //!< Opens the listener. Returns false when the system refuses.
        bool open( )
        {
            sockaddr_in addr{ };
            const NativeSocket sock{ open_listener( addr, 8 ) };
            if ( sock == NO_SOCKET )
                return false;

            handle = static_cast<SOCKETHANDLE>(sock);
            address.from_sockaddr( addr );
            return true;
        }

        ~LoopListener( )
        {
            if ( handle != areg::InvalidSocketHandle )
                areg::socket_close( handle );
        }
    };

    //!< Sets or clears the non-blocking mode of the socket.
    inline bool set_nonblocking( SOCKETHANDLE sock, bool nonBlocking )
    {
#if defined(_WIN32)
        u_long mode{ nonBlocking ? 1u : 0u };
        return ( ::ioctlsocket( static_cast<SOCKET>(sock), static_cast<long>(FIONBIO), &mode ) == 0 );
#else   // defined(_WIN32)
        const int flags{ ::fcntl( static_cast<int>(sock), F_GETFL, 0 ) };
        if ( flags == -1 )
            return false;

        const int wanted{ nonBlocking ? ( flags | O_NONBLOCK ) : ( flags & ~O_NONBLOCK ) };
        return ( ::fcntl( static_cast<int>(sock), F_SETFL, wanted ) != -1 );
#endif  // defined(_WIN32)
    }

    //!< Receives exactly 'length' bytes from the plain socket. Returns false on an error or end of stream.
    inline bool read_exact( SOCKETHANDLE sock, uint8_t * buffer, uint32_t length )
    {
        uint32_t total{ 0u };
        while ( total < length )
        {
            const int got{ static_cast<int>(::recv( static_cast<NativeSocket>(sock), reinterpret_cast<char *>(buffer + total), static_cast<int>(length - total), 0 )) };
            if ( got <= 0 )
                return false;

            total += static_cast<uint32_t>(got);
        }

        return true;
    }

    //!< Sends 4 bytes and checks the peer receives them.
    inline bool round_trip( SOCKETHANDLE from, SOCKETHANDLE to )
    {
        const uint8_t data[4]{ 1u, 2u, 3u, 4u };
        uint8_t received[4]{ };
        return ( areg::send_data( from, data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) )
            && read_exact( to, received, sizeof( received ) )
            && ( std::memcmp( data, received, sizeof( data ) ) == 0 );
    }

    //!< Sets the receive mode of the calling thread and restores the previous one.
    class ModeScope
    {
    public:
        explicit ModeScope( areg::ReceiveMode mode )
            : mSaved( areg::receive_mode( ) )
        {
            areg::set_receive_mode( mode );
        }

        ~ModeScope( )
        {
            areg::set_receive_mode( mSaved );
        }

        ModeScope( const ModeScope & ) = delete;
        ModeScope & operator = ( const ModeScope & ) = delete;

    private:
        areg::ReceiveMode mSaved;
    };
}

#endif  // AREG_TESTS_FAULT_INJECT_FAULTSOCKETPAIR_HPP
