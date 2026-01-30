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
#include "areg/base/GEGlobal.h"
#include "aregextend/service/ServiceApplicationBase.hpp"

#include "areg/base/SyncObjects.hpp"
#include "aregextend/console/OptionParser.hpp"
#include "aregextend/service/NESystemService.hpp"
#include "mtrouter/app/NEMultitargetRouterSettings.hpp"
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
class MultitargetRouter final : public ServiceApplicationBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   MultitargetRouter::eRouterOptions
     *          The command to handle the message router.
     **/
    enum class eRouterOptions : int32_t
    {
          CMD_RouterUndefined   = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Undefined)  //!< Undefined command.
        , CMD_RouterPrintHelp   = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Help)       //!< Print help.
        , CMD_RouterLoad        = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Load)       //!< Start the service by loading initialization instructions from configuration file.
        , CMD_RouterVerbose     = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Verbose)    //!< Display data rate information if possible. Functions only with extended features
        , CMD_RouterUninstall   = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Uninstall)  //!< Uninstall as a service. Valid only as a command line option in Windows OS
        , CMD_RouterInstall     = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Install)    //!< Install as service. Valid only as a command line option in Windows OS
        , CMD_RouterService     = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Service)    //!< Start router as a service. Valid only as a command line option in Windows OS
        , CMD_RouterConsole     = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Console)    //!< Run as console application. Valid only as a command line option
        , CMD_RouterPause       = static_cast<int32_t>(NESystemService::eServiceOption::CMD_Custom)     //!< Pause router.
        , CMD_RouterRestart                                                                             //!< Start / Restart router.
        , CMD_RouterInstances                                                                           //!< Display list of connected instances.
        , CMD_RouterSilent                                                                              //!< Silent mode, no data rate is displayed.
        , CMD_RouterQuit                                                                                //!< Quit router.
    };

    /**
     * \brief   The setup to validate input options of the message router.
     **/
    static const OptionParser::sOptionSetup ValidOptions[ ];

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of multi-cast router (MCR).
     **/
    static MultitargetRouter & getInstance( void );

    /**
     * \brief   Outputs the specified message on the console.
     *          The method is valid only for console application compiled
     *          with Areg Extended features.
     *          Otherwise, the method ignores request to output message.
     * \param   status  The status message to print on console.
     **/
    static void printStatus(const String& status);

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor and destructor.
     **/
    MultitargetRouter( void );

    virtual ~MultitargetRouter( void ) = default;

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
    virtual void printHelp( bool isCmdLine ) override;

    /**
     * \brief   Triggered to start the console service.
     **/
    virtual void startConsoleService( void ) override;

    /**
     * \brief   Stops the consoler service.
     **/
    virtual void stopConsoleService( void ) override;

    /**
     * \brief   Triggered to receive a function to validate and check the input option values.
     **/
    virtual Console::CallBack getOptionCheckCallback( void ) const override;

    /**
     * \brief   Triggered if need to run console with extended features.
     *          In extended feature, the console can output message at any position on the screen.
     **/
    virtual void runConsoleInputExtended( void ) override;

    /**
     * \brief   Triggered if need to run console with simple (not extended) features.
     **/
    virtual void runConsoleInputSimple( void ) override;

/************************************************************************/
// ServiceApplicationBase protected overrides
/************************************************************************/
    /**
     * \brief   Returns list of the options to validate contained in the pair object,
     *          where the first entry is the pointer to the list and second entry is
     *          the number of elements in the list
     **/
    virtual std::pair<const OptionParser::sOptionSetup*, int> getAppOptions(void) const override;

    /**
     * \brief   Returns the UNICODE name of the service application.
     **/
    virtual wchar_t* getServiceNameW(void) const override;

    /**
     * \brief   Returns the ASCII name of the service application.
     **/
    virtual char* getServiceNameA(void) const override;

    /**
     * \brief   Returns the UNICODE display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    virtual wchar_t* getServiceDisplayNameW(void) const override;

    /**
     * \brief   Returns the ASCII display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    virtual char* getServiceDisplayNameA(void) const override;

    /**
     * \brief   Returns the UNICODE description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual wchar_t* getServiceDescriptionW(void) const override;

    /**
     * \brief   Returns the ASCII description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual char* getServiceDescriptionA(void) const override;

    /**
     * \brief   Returns the type of the remote service.
     *          Valid only for Areg SDK services.
     **/
    virtual NERemoteService::eRemoteServices getServiceType(void) const override;

    /**
     * \brief   Returns the type of the connection of the remote services.
     *          Valid only for Areg SDK services.
     **/
    virtual NERemoteService::eConnectionTypes getConnectionType(void) const override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns the list of connected instances.
     **/
    inline const NEService::MapInstances & getConnetedInstances( void ) const;

    /**
     * \brief   Returns instance of message router service.
     **/
    inline MultitargetRouter & self( void );

    /**
     * \brief   Checks the command typed on console. Relevant only if it runs as a console application.
     * \param   cmd     The command typed on the console.
     * \return  Returns true if command is recognized. Otherwise, returns false.
     **/
    static bool _checkCommand(const String& cmd);

    /**
     * \brief   Output on console the title.
     **/
    static void _outputTitle( void );

    /**
     * \brief   Prints info on console.
     **/
    static void _outputInfo( const String & info );

    /**
     * \brief   Outputs on console the information about connected instances.
     **/
    static void _outputInstances( const NEService::MapInstances & instances );

    /**
     * \brief   Sets verbose or silent mode to output data rate.
     *          The feature is available only if compile with enabled extended features.
     *          Otherwise, it outputs error message and nothing happens.
     **/
    static void _setVerboseMode( bool makeVerbose );

    /**
     * \brief   Call to clean all message outputs like help, prompt, etc.
     *          Normally, help is the largest message.
     **/
    static void _cleanHelp(void);

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
    DECLARE_NOCOPY_NOMOVE( MultitargetRouter );
};

//////////////////////////////////////////////////////////////////////////
// MultitargetRouter class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const NEService::MapInstances & MultitargetRouter::getConnetedInstances( void ) const
{
    return mServiceServer.getInstances( );
}

inline MultitargetRouter & MultitargetRouter::self( void )
{
    return (*this);
}

#endif  // AREG_mtrouter_APP_MULTITARGETROUTER_HPP
