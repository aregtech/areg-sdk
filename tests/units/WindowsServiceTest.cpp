/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/WindowsServiceTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of mtrouter and logcollector as Windows services:
 *              install, start, pause, continue, stop and uninstall through the service
 *              control manager. They run only with the rights to create a service, and only
 *              when no service of that name is installed.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"

#if defined(_WIN32)

#include "areg/appbase/AppDefs.hpp"
#include "areg/appbase/Application.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif  // NOMINMAX
#include <Windows.h>
#include <winsvc.h>

#include <ostream>
#include <string>
#include <vector>

namespace
{
    //!< The limit of a wait for a service state or a process.
    constexpr DWORD     WAIT_MS { 60'000u };

    //!< A service application of the build and the call that starts it.
    struct ServiceApp
    {
        const wchar_t *     executable;
        const wchar_t *     service;
        bool              (*start)( );
    };

    //!< Returns the full path of a file in the folder of this test executable.
    std::wstring beside_test( const wchar_t * fileName )
    {
        wchar_t path[MAX_PATH]{ };
        const DWORD length{ ::GetModuleFileNameW( nullptr, path, MAX_PATH ) };
        std::wstring result( path, length );
        const std::size_t slash{ result.find_last_of( L"\\/" ) };
        result.resize( slash == std::wstring::npos ? 0u : slash + 1u );
        result += fileName;
        return result;
    }

    //!< Runs the executable with the option and returns its exit code, or -1 if it did not run.
    int32_t run_option( const std::wstring & executable, const wchar_t * option )
    {
        std::wstring command{ L"\"" + executable + L"\" " + option };
        std::vector<wchar_t> line( command.begin( ), command.end( ) );
        line.push_back( L'\0' );

        STARTUPINFOW startup{ };
        startup.cb = sizeof( startup );
        PROCESS_INFORMATION process{ };
        if ( ::CreateProcessW( executable.c_str( ), line.data( ), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup, &process ) == FALSE )
            return -1;

        DWORD exitCode{ static_cast<DWORD>(-1) };
        if ( ::WaitForSingleObject( process.hProcess, WAIT_MS ) == WAIT_OBJECT_0 )
        {
            static_cast<void>( ::GetExitCodeProcess( process.hProcess, &exitCode ) );
        }

        ::CloseHandle( process.hThread );
        ::CloseHandle( process.hProcess );
        return static_cast<int32_t>(exitCode);
    }

    //!< Marks the change of the state a test waits for.
    VOID CALLBACK _on_state_change( PVOID /*context*/ )
    {
    }

    //!< Waits until the service reaches the state, notified by the service control manager.
    bool wait_state( SC_HANDLE service, DWORD state, DWORD mask )
    {
        const ULONGLONG deadline{ ::GetTickCount64( ) + WAIT_MS };
        for ( ULONGLONG now = ::GetTickCount64( ); now < deadline; now = ::GetTickCount64( ) )
        {
            SERVICE_STATUS status{ };
            if ( ::QueryServiceStatus( service, &status ) == FALSE )
                return false;

            if ( status.dwCurrentState == state )
                return true;

            SERVICE_NOTIFYW notify{ };
            notify.dwVersion         = SERVICE_NOTIFY_STATUS_CHANGE;
            notify.pfnNotifyCallback = &_on_state_change;
            if ( ::NotifyServiceStatusChangeW( service, mask, &notify ) != ERROR_SUCCESS )
                return false;

            // The notification arrives as a call to this thread while it waits alertable.
            static_cast<void>( ::SleepEx( static_cast<DWORD>(deadline - now), TRUE ) );
        }

        return false;
    }

    //!< Removes the service when the test ends, whatever its outcome.
    class ServiceCleanup
    {
    public:
        ServiceCleanup( SC_HANDLE manager, const wchar_t * name )
            : mManager  ( manager )
            , mName     ( name )
        {
        }

        ~ServiceCleanup( )
        {
            SC_HANDLE service{ ::OpenServiceW( mManager, mName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE ) };
            if ( service != nullptr )
            {
                SERVICE_STATUS status{ };
                if ( ::ControlService( service, SERVICE_CONTROL_STOP, &status ) != FALSE )
                {
                    static_cast<void>( wait_state( service, SERVICE_STOPPED, SERVICE_NOTIFY_STOPPED ) );
                }

                static_cast<void>( ::DeleteService( service ) );
                ::CloseServiceHandle( service );
            }

            ::CloseServiceHandle( mManager );
        }

        ServiceCleanup( const ServiceCleanup & ) = delete;
        ServiceCleanup & operator = ( const ServiceCleanup & ) = delete;

    private:
        SC_HANDLE       mManager;
        const wchar_t * mName;
    };

    //!< Prints the executable name of the parameter.
    void PrintTo( const ServiceApp & app, std::ostream * os )
    {
        for ( const wchar_t * ch = app.executable; *ch != L'\0'; ++ ch )
        {
            *os << static_cast<char>(*ch);
        }
    }

    bool _start_router( )
    {
        return areg::Application::start_router_service( );
    }

    bool _start_logger( )
    {
        return areg::Application::start_logging_service( );
    }

    //!< Returns true if this process loaded an ASan runtime that is not in the folder of the test.
    bool asan_runtime_not_beside( )
    {
        constexpr const wchar_t * RUNTIMES[]
        {
              L"clang_rt.asan_dynamic-x86_64.dll"
            , L"clang_rt.asan_dbg_dynamic-x86_64.dll"
            , L"clang_rt.asan_dynamic-i386.dll"
            , L"clang_rt.asan_dbg_dynamic-i386.dll"
        };

        for ( const wchar_t * runtime : RUNTIMES )
        {
            if ( (::GetModuleHandleW( runtime ) != nullptr) && (::GetFileAttributesW( beside_test( runtime ).c_str( ) ) == INVALID_FILE_ATTRIBUTES) )
                return true;
        }

        return false;
    }

    class WindowsServiceTest : public ::testing::TestWithParam<ServiceApp>
    {
    };
}

TEST_P( WindowsServiceTest, installed_service_starts_pauses_continues_stops_and_is_removed )
{
    const ServiceApp & app{ GetParam( ) };
    const std::wstring executable{ beside_test( app.executable ) };
    if ( ::GetFileAttributesW( executable.c_str( ) ) == INVALID_FILE_ATTRIBUTES )
    {
        GTEST_SKIP( ) << "the service application is not built beside the test";
    }

    SC_HANDLE reader{ ::OpenSCManagerW( nullptr, nullptr, SC_MANAGER_CONNECT ) };
    ASSERT_NE( reader, nullptr );
    SC_HANDLE existing{ ::OpenServiceW( reader, app.service, SERVICE_QUERY_STATUS ) };
    if ( existing != nullptr )
    {
        ::CloseServiceHandle( existing );
        ::CloseServiceHandle( reader );
        GTEST_SKIP( ) << "a service of this name is installed on this machine";
    }

    SC_HANDLE manager{ ::OpenSCManagerW( nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE ) };
    if ( manager == nullptr )
    {
        // Without the rights the install fails and leaves no service.
        EXPECT_NE( run_option( executable, L"--install" ), 0 );
        SC_HANDLE none{ ::OpenServiceW( reader, app.service, SERVICE_QUERY_STATUS ) };
        if ( none != nullptr )
        {
            ::CloseServiceHandle( none );
        }

        ::CloseServiceHandle( reader );
        EXPECT_EQ( none, nullptr );
        GTEST_SKIP( ) << "creating a service needs administrator rights";
    }

    ::CloseServiceHandle( reader );

    if ( asan_runtime_not_beside( ) )
    {
        ::CloseServiceHandle( manager );
        GTEST_SKIP( ) << "a service finds the ASan runtime only in the folder of the service application";
    }

    ServiceCleanup cleanup( manager, app.service );

    // A second install finds the service and keeps it.
    ASSERT_EQ( run_option( executable, L"--install" ), 0 );
    ASSERT_EQ( run_option( executable, L"--install" ), 0 );

    SC_HANDLE service{ ::OpenServiceW( manager, app.service, SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG | SERVICE_PAUSE_CONTINUE | SERVICE_STOP ) };
    ASSERT_NE( service, nullptr );

    DWORD needed{ 0u };
    static_cast<void>( ::QueryServiceConfigW( service, nullptr, 0u, &needed ) );
    std::vector<uint8_t> config( needed );
    ASSERT_NE( ::QueryServiceConfigW( service, reinterpret_cast<QUERY_SERVICE_CONFIGW *>(config.data( )), needed, &needed ), FALSE );
    EXPECT_EQ( reinterpret_cast<QUERY_SERVICE_CONFIGW *>(config.data( ))->dwStartType, static_cast<DWORD>(SERVICE_AUTO_START) );

    static_cast<void>( ::QueryServiceConfig2W( service, SERVICE_CONFIG_FAILURE_ACTIONS, nullptr, 0u, &needed ) );
    std::vector<uint8_t> actions( needed );
    ASSERT_NE( ::QueryServiceConfig2W( service, SERVICE_CONFIG_FAILURE_ACTIONS, actions.data( ), needed, &needed ), FALSE );
    const SERVICE_FAILURE_ACTIONSW & failures{ *reinterpret_cast<SERVICE_FAILURE_ACTIONSW *>(actions.data( )) };
    ASSERT_EQ( failures.cActions, 5u );
    for ( DWORD i = 0u; i < failures.cActions; ++ i )
    {
        EXPECT_EQ( failures.lpsaActions[i].Type, SC_ACTION_RESTART );
    }

    // A stopped service is started, a running one is left running.
    EXPECT_TRUE( app.start( ) );
    ASSERT_TRUE( wait_state( service, SERVICE_RUNNING, SERVICE_NOTIFY_RUNNING ) );
    EXPECT_TRUE( app.start( ) );

    // A paused service is continued.
    SERVICE_STATUS status{ };
    ASSERT_NE( ::ControlService( service, SERVICE_CONTROL_PAUSE, &status ), FALSE );
    ASSERT_TRUE( wait_state( service, SERVICE_PAUSED, SERVICE_NOTIFY_PAUSED ) );
    EXPECT_TRUE( app.start( ) );
    ASSERT_TRUE( wait_state( service, SERVICE_RUNNING, SERVICE_NOTIFY_RUNNING ) );

    ASSERT_NE( ::ControlService( service, SERVICE_CONTROL_STOP, &status ), FALSE );
    ASSERT_TRUE( wait_state( service, SERVICE_STOPPED, SERVICE_NOTIFY_STOPPED ) );
    ::CloseServiceHandle( service );

    EXPECT_EQ( run_option( executable, L"--uninstall" ), 0 );
    SC_HANDLE removed{ ::OpenServiceW( manager, app.service, SERVICE_QUERY_STATUS ) };
    if ( removed != nullptr )
    {
        // A service marked for deletion stays visible until its last handle closes.
        SERVICE_STATUS last{ };
        EXPECT_EQ( ::QueryServiceStatus( removed, &last ) != FALSE ? last.dwCurrentState : static_cast<DWORD>(SERVICE_STOPPED), static_cast<DWORD>(SERVICE_STOPPED) );
        ::CloseServiceHandle( removed );
    }
}

INSTANTIATE_TEST_SUITE_P( Applications, WindowsServiceTest, ::testing::Values(
      ServiceApp{ L"mtrouter.exe"    , areg::ROUTER_SERVICE_NAME_WIDE, &_start_router }
    , ServiceApp{ L"logcollector.exe", areg::LOGGER_SERVICE_NAME_WIDE, &_start_logger } ) );

#endif  // defined(_WIN32)
