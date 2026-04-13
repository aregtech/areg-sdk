#ifndef AREG_mtrouter_APP_MULTITARGETROUTER_HPP
#define AREG_mtrouter_APP_MULTITARGETROUTER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/MultitargetRouter.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Multi-cast routing to run as process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "aregextend/service/ServiceApplicationBase.hpp"

#include "areg/base/SyncPrimitives.hpp"
#include "aregextend/console/OptionParser.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"
#include "mtrouter/app/MTRouterNames.hpp"
#include "mtrouter/service/RouterServerService.hpp"

#include <utility>

class Console;

//////////////////////////////////////////////////////////////////////////
// MultitargetRouter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message routing service is a separate process, which receives and routs messages
 *          to the connected servicing components. Applications connect to message routing service via
 *          TCP/IP protocol. The message router distributes the IPC message to the targets.
 **/
class MultitargetRouter final : public areg::ext::ServiceApplicationBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   MultitargetRouter::RouterOption
     *          The command to handle the message router.
     **/
    enum class RouterOption : int32_t
    {
          CMD_RouterUndefined   = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Undefined)  //!< Undefined command.
        , CMD_RouterPrintHelp   = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Help)       //!< Print help.
        , CMD_RouterLoad        = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Load)       //!< Start the service by loading initialization instructions from configuration file.
        , CMD_RouterVerbose     = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Verbose)    //!< Display data rate information if possible. Functions only with extended features
        , CMD_RouterUninstall   = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Uninstall)  //!< Uninstall as a service. Valid only as a command line option in Windows OS
        , CMD_RouterInstall     = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Install)    //!< Install as service. Valid only as a command line option in Windows OS
        , CMD_RouterService     = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Service)    //!< Start router as a service. Valid only as a command line option in Windows OS
        , CMD_RouterConsole     = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Console)    //!< Run as console application. Valid only as a command line option
        , CMD_RouterPause       = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Custom)     //!< Pause router.
        , CMD_RouterRestart                                                                             //!< Start / Restart router.
        , CMD_RouterInstances                                                                           //!< Display list of connected instances.
        , CMD_RouterSilent                                                                              //!< Silent mode, no data rate is displayed.
        , CMD_RouterThreads                                                                             //!< Display per-client thread pair count.
        , CMD_RouterQuit                                                                                //!< Quit router.
    };

    static inline constexpr const char* as_string(MultitargetRouter::RouterOption value);

    /**
     * \brief   The setup to validate input options of the message router.
     **/
    static const areg::ext::OptionParser::OptionSetup ValidOptions[ ];

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of multi-cast router (MCR).
     **/
    static MultitargetRouter & instance();

    /**
     * \brief   Outputs the specified message on the console.
     *          The method is valid only for console application compiled
     *          with Areg Extended features.
     *          Otherwise, the method ignores request to output message.
     * \param   status  The status message to print on console.
     **/
    static void print_status(const areg::String& status);

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    MultitargetRouter();

    ~MultitargetRouter() override = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// SystemServiceBase protected overrides
/************************************************************************/

    /**
     * \brief   Triggered to print the help message on console.
     * \param   isCmdLine   Flag indicating whether it should print the help
     *                      of using service in command line or help of user input commands.
     *                      If 'true', the printing message is about using the service in
     *                      command line. Otherwise, if application expects user inputs, prints
     *                      the help of command options.
     **/
    void print_help( bool isCmdLine ) final;

    /**
     * \brief   Triggered to start the console service.
     **/
    void start_console_service() final;

    /**
     * \brief   Stops the console service.
     **/
    void stop_console_service() final;

    /**
     * \brief   Triggered to receive a function to validate and check the input option values.
     **/
    areg::ext::Console::CallBack option_check_callback() const final;

    /**
     * \brief   Triggered if need to run console with extended features.
     *          In extended feature, the console can output message at any position on the screen.
     **/
    void run_console_input_extended() final;

    /**
     * \brief   Triggered if need to run console with simple (not extended) features.
     **/
    void run_console_input_simple() final;

/************************************************************************/
// ServiceApplicationBase protected overrides
/************************************************************************/
    /**
     * \brief   Returns list of the options to validate contained in the pair object,
     *          where the first entry is the pointer to the list and second entry is
     *          the number of elements in the list
     **/
    std::pair<const areg::ext::OptionParser::OptionSetup*, int32_t> app_options() const final;

    /**
     * \brief   Returns the UNICODE name of the service application.
     **/
    wchar_t* service_name_w() const final;

    /**
     * \brief   Returns the ASCII name of the service application.
     **/
    char* service_name_a() const final;

    /**
     * \brief   Returns the UNICODE display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    wchar_t* service_display_name_w() const final;

    /**
     * \brief   Returns the ASCII display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    char* service_display_name_a() const final;

    /**
     * \brief   Returns the UNICODE description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    wchar_t* service_description_w() const final;

    /**
     * \brief   Returns the ASCII description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    char* service_description_a() const final;

    /**
     * \brief   Returns the type of the remote service.
     *          Valid only for Areg SDK services.
     **/
    areg::RemoteServiceKind service_type() const final;

    /**
     * \brief   Returns the type of the connection of the remote services.
     *          Valid only for Areg SDK services.
     **/
    areg::ConnectionType connection_type() const final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns the list of connected instances.
     **/
    inline const areg::MapInstances & conneted_instances() const;

    /**
     * \brief   Returns instance of message router service.
     **/
    inline MultitargetRouter & self();

    /**
     * \brief   Checks the command typed on console. Relevant only if it runs as a console application.
     * \param   cmd     The command typed on the console.
     * \return  Returns true if command is recognized. Otherwise, returns false.
     **/
    static bool _check_command(const areg::String& cmd);

    /**
     * \brief   Output on console the title.
     **/
    static void _output_title();

    /**
     * \brief   Prints info on console.
     **/
    static void _output_info( const areg::String & info );

    /**
     * \brief   Outputs on console the information about connected instances.
     **/
    static void _output_instances( const areg::MapInstances & instances );

    /**
     * \brief   Sets verbose or silent mode to output data rate.
     *          The feature is available only if compile with enabled extended features.
     *          Otherwise, it outputs error message and nothing happens.
     **/
    static void _set_verbose_mode( bool makeVerbose );

    /**
     * \brief   Call to clean all message outputs like help, prompt, etc.
     *          Normally, help is the largest message.
     **/
    static void _clean_help();

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of message router service server to accept connections from applications.
     **/
    RouterServerService mServiceServer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( MultitargetRouter );
};

//////////////////////////////////////////////////////////////////////////
// MultitargetRouter class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const areg::MapInstances & MultitargetRouter::conneted_instances() const
{
    return mServiceServer.instances( );
}

inline MultitargetRouter & MultitargetRouter::self()
{
    return (*this);
}

inline constexpr const char* MultitargetRouter::as_string(MultitargetRouter::RouterOption value)
{
    switch (value)
    {
    case MultitargetRouter::RouterOption::CMD_RouterUndefined:
        return "MultitargetRouter::RouterOption::CMD_RouterUndefined";
    case MultitargetRouter::RouterOption::CMD_RouterPrintHelp:
        return "MultitargetRouter::RouterOption::CMD_RouterPrintHelp";
    case MultitargetRouter::RouterOption::CMD_RouterLoad:
        return "MultitargetRouter::RouterOption::CMD_RouterLoad";
    case MultitargetRouter::RouterOption::CMD_RouterVerbose:
        return "MultitargetRouter::RouterOption::CMD_RouterVerbose";
    case MultitargetRouter::RouterOption::CMD_RouterUninstall:
        return "MultitargetRouter::RouterOption::CMD_RouterUninstall";
    case MultitargetRouter::RouterOption::CMD_RouterInstall:
        return "MultitargetRouter::RouterOption::CMD_RouterInstall";
    case MultitargetRouter::RouterOption::CMD_RouterService:
        return "MultitargetRouter::RouterOption::CMD_RouterService";
    case MultitargetRouter::RouterOption::CMD_RouterConsole:
        return "MultitargetRouter::RouterOption::CMD_RouterConsole";
    case MultitargetRouter::RouterOption::CMD_RouterPause:
        return "MultitargetRouter::RouterOption::CMD_RouterPause";
    case MultitargetRouter::RouterOption::CMD_RouterRestart:
        return "MultitargetRouter::RouterOption::CMD_RouterRestart";
    case MultitargetRouter::RouterOption::CMD_RouterInstances:
        return "MultitargetRouter::RouterOption::CMD_RouterInstances";
    case MultitargetRouter::RouterOption::CMD_RouterSilent:
        return "MultitargetRouter::RouterOption::CMD_RouterSilent";
    case MultitargetRouter::RouterOption::CMD_RouterThreads:
        return "MultitargetRouter::RouterOption::CMD_RouterThreads";
    case MultitargetRouter::RouterOption::CMD_RouterQuit:
        return "MultitargetRouter::RouterOption::CMD_RouterQuit";
    default:
        ASSERT(false);
        return "ERR: Undefined MultitargetRouter::RouterOption value!";
    }


}

#endif  // AREG_mtrouter_APP_MULTITARGETROUTER_HPP
