#ifndef AREG_TESTS_UNITS_APPLICATIONRESTARTTESTSUPPORT_HPP
#define AREG_TESTS_UNITS_APPLICATIONRESTARTTESTSUPPORT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ApplicationRestartTestSupport.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. Files, configurations and listening sockets of
 *              the tests that run several application sessions in one process.
 ************************************************************************/

#include "units/ChildProcess.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Socket.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#if defined(POSIX)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#elif defined(WINDOWS)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#endif

namespace restart_test
{
    //!< The longest wait of a test for an event that proves a state.
    constexpr uint32_t  WAIT_MS{ 30'000u };

    //!< The names of the threads the application starts.
    constexpr const char * const MANAGER_THREADS[]
    {
          "_AREG_TIMER_THREAD_NAME_"
        , "_AREG_WATCHDOG_THREAD_NAME_"
        , "_AREG_SERVICE_MANAGER_THREAD_"
        , "_AREG_LOGGING_THREAD_"
    };

    //!< The address of the service manager thread.
    constexpr const char * const SERVICE_THREAD{ MANAGER_THREADS[2] };

    //!< Returns true if the thread with the name is registered.
    inline bool thread_exists( const char * name )
    {
        return (areg::Thread::find_by_address( areg::ThreadAddress( name ) ) != nullptr);
    }

    //!< Ends the child if the system differs from a process that has just entered main().
    inline void check_system_is_stopped( )
    {
        AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Stopped );
        AREG_CHILD_CHECK( areg::Application::is_servicing_ready( ) == false );
        AREG_CHILD_CHECK( areg::Application::is_servicing_available( ) == false );
        for ( const char * name : MANAGER_THREADS )
        {
            AREG_CHILD_CHECK( thread_exists( name ) == false );
        }
#if AREG_LOGGING
        AREG_CHILD_CHECK( areg::is_started( ) == false );
#endif
    }

    //!< The content of one configuration file.
    struct ConfigText
    {
        std::string logFile     { };    //!< The log file. Logging is off if empty.
        std::string layout      { "%m%n" }; //!< The layout of a message.
        uint16_t    routerPort  { 0u }; //!< The port of the router. The router is off if 0.
        uint16_t    loggerPort  { 0u }; //!< The port of the log collector. Remote logging is off if 0.
        std::string extra       { };    //!< More lines of the file.

        //!< Returns the text of the file.
        std::string text( ) const
        {
            const bool router{ routerPort != 0u };
            const bool logger{ loggerPort != 0u };
            const bool logging{ (logFile.empty( ) == false) || logger };
            std::string result;
            result += "config::*::version = 2.0.0\n";
            result += std::string( "log::*::enable = " ) + (logging ? "true" : "false") + "\n";
            result += std::string( "log::*::enable::file = " ) + (logFile.empty( ) ? "false" : "true") + "\n";
            result += std::string( "log::*::enable::remote = " ) + (logger ? "true" : "false") + "\n";
            result += "log::*::enable::output = false\n";
            result += "log::*::enable::db = false\n";
            result += "log::*::file::append = false\n";
            if ( logFile.empty( ) == false )
            {
                result += "log::*::file::location = " + logFile + "\n";
            }

            result += "log::*::layout::message = " + layout + "\n";
            result += "log::*::layout::enter = >%m%n\n";
            result += "log::*::layout::exit = <%m%n\n";
            result += "log::*::remote::queue = 16\n";
            result += "log::*::scope::* = NOTSET\n";
            result += "service::*::list = router | logger\n";
            result += std::string( "router::*::enable::tcpip = " ) + (router ? "true" : "false") + "\n";
            result += "router::*::address::tcpip = 127.0.0.1\n";
            result += "router::*::port::tcpip = " + std::to_string( router ? routerPort : 8181u ) + "\n";
            result += std::string( "logger::*::enable::tcpip = " ) + (logger ? "true" : "false") + "\n";
            result += "logger::*::address::tcpip = 127.0.0.1\n";
            result += "logger::*::port::tcpip = " + std::to_string( logger ? loggerPort : 8282u ) + "\n";
            result += extra;
            return result;
        }
    };

    //!< Returns the content of the file, empty if it cannot be read.
    inline std::string read_text( const std::string & path )
    {
        std::ifstream input( path, std::ios::binary );
        return std::string( std::istreambuf_iterator<char>( input ), std::istreambuf_iterator<char>( ) );
    }

    //!< Returns true if the text contains the part.
    inline bool contains( const std::string & text, const char * part )
    {
        return (text.find( part ) != std::string::npos);
    }

    //!< A directory of one child process, deleted with the object.
    class Sandbox
    {
    public:
        Sandbox( )
            : mDirectory( )
        {
            std::error_code error;
            const std::filesystem::path temp{ std::filesystem::absolute( testing::TempDir( ), error ) };
            AREG_CHILD_CHECK( !error );
            mDirectory = temp / ("areg_restart_" + std::to_string( areg::Process::instance( ).id( ) ));
            std::filesystem::remove_all( mDirectory, error );
            AREG_CHILD_CHECK( std::filesystem::create_directories( mDirectory, error ) );
        }

        ~Sandbox( )
        {
            std::error_code error;
            std::filesystem::remove_all( mDirectory, error );
        }

        //!< Returns the full path of a file in the directory.
        std::string path( const char * name ) const
        {
            return (mDirectory / name).string( );
        }

        //!< Writes the file and returns its full path.
        std::string write( const char * name, const std::string & content ) const
        {
            const std::string result{ path( name ) };
            std::ofstream output( result, std::ios::binary | std::ios::trunc );
            output << content;
            output.close( );
            AREG_CHILD_CHECK( output.fail( ) == false );
            return result;
        }

    private:
        std::filesystem::path mDirectory;
    };

    //!< A TCP listener on a free port of the loopback interface.
    class TcpPort
    {
    public:
        TcpPort( )
            : mSocket   ( )
            , mAddress  ( )
            , mAccepted ( )
        {
            AREG_CHILD_CHECK( areg::socket_initialize( ) );
            mSocket = areg::Socket::SocketHandle( areg::socket_create( ) );
            AREG_CHILD_CHECK( areg::is_valid_socket( mSocket.value( ) ) );

            sockaddr_in local{ };
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
            AREG_CHILD_CHECK( ::bind( mSocket.value( ), reinterpret_cast<sockaddr *>(&local), sizeof( local ) ) == 0 );

#if defined(WINDOWS)
            int length{ sizeof( local ) };
#else
            socklen_t length{ sizeof( local ) };
#endif
            AREG_CHILD_CHECK( ::getsockname( mSocket.value( ), reinterpret_cast<sockaddr *>(&local), &length ) == 0 );
            mAddress.from_sockaddr( local );
            AREG_CHILD_CHECK( areg::server_listen( mSocket.value( ), 8 ) );
        }

        //!< Returns the port number.
        uint16_t port( ) const
        {
            return mAddress.host_port( );
        }

        //!< Returns true if a client connected within the time.
        bool client_connects( uint32_t waitMs )
        {
            areg::SocketMultiplexer monitor;
            if ( monitor.register_socket( mSocket.value( ), true ) == false )
                return false;

            areg::SocketAddress peer;
            const SOCKETHANDLE accepted{ areg::server_accept( monitor, mSocket.value( ), &peer, static_cast<int32_t>(waitMs) ) };
            if ( areg::is_valid_socket( accepted ) == false )
                return false;

            mAccepted.emplace_back( accepted, peer );
            return true;
        }

    private:
        areg::Socket::SocketHandle      mSocket;
        areg::SocketAddress             mAddress;
        std::vector<areg::SocketAccepted> mAccepted;
    };
}

#endif  // AREG_TESTS_UNITS_APPLICATIONRESTARTTESTSUPPORT_HPP
