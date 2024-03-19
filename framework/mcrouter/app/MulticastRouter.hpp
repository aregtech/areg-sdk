#ifndef AREG_MCROUTER_APP_MULTICASTROUTER_HPP
#define AREG_MCROUTER_APP_MULTICASTROUTER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/MulticastRouter.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing to run as process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "extend/service/ServiceApplicationBase.hpp"

#include "areg/base/SynchObjects.hpp"
#include "mcrouter/app/NEMulticastRouterSettings.hpp"
#include "mcrouter/service/RouterServerService.hpp"
#include "extend/console/OptionParser.hpp"

#include <utility>

class Console;

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message routing service is a separate process, which receives and routs messages
 *          to the connected servicing components. Applications connect to message routing service via
 *          TCP/IP protocol. The message router distributes the IPC message to the targets.
 **/
class MulticastRouter final : public ServiceApplicationBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   MulticastRouter::eRouterOptions
     *          The command to handle the message router.
     **/
    enum class eRouterOptions : int32_t
    {
          CMD_RouterUndefined   //!< Undefined command.
        , CMD_RouterPause       //!< Pause router.
        , CMD_RouterRestart     //!< Restart router.
        , CMD_RouterInstances   //!< Display list of connected instances.
        , CMD_RouterVerbose     //!< Display data rate information if possible. Functions only with extended features
        , CMD_RouterSilent      //!< Silent mode, no data rate is displayed.
        , CMD_RouterPrintHelp   //!< Print help.
        , CMD_RouterQuit        //!< Quit router.
        , CMD_RouterConsole     //!< Run as console application. Valid only as a command line option
        , CMD_RouterInstall     //!< Install as service. Valid only as a command line option in Windows OS
        , CMD_RouterUninstall   //!< Uninstall as a service. Valid only as a command line option in Windows OS
        , CMD_RouterService     //!< Start router as a service. Valid only as a command line option in Windows OS
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
    static MulticastRouter & getInstance( void );

    /**
     * \brief   Outputs the specified message on the console.
     *          The method is valid only for console application compiled
     *          with AREG Extended features.
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
    MulticastRouter( void );

    virtual ~MulticastRouter( void ) = default;

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
     *          Valid only for AREG SDK services.
     **/
    virtual NERemoteService::eRemoteServices getServiceType(void) const override;

    /**
     * \brief   Returns the type of the connection of the remote services.
     *          Valid only for AREG SDK services.
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
    inline MulticastRouter & self( void );

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
    DECLARE_NOCOPY_NOMOVE( MulticastRouter );
};

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const NEService::MapInstances & MulticastRouter::getConnetedInstances( void ) const
{
    return mServiceServer.getInstances( );
}

inline MulticastRouter & MulticastRouter::self( void )
{
    return (*this);
}

#endif  // AREG_MCROUTER_APP_MULTICASTROUTER_HPP
