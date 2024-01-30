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
#include "extend/service/SystemServiceBase.hpp"

#include "areg/base/SynchObjects.hpp"
#include "mcrouter/app/NEMulticastRouterSettings.hpp"
#include "mcrouter/tcp/RouterServerService.hpp"
#include "extend/console/OptionParser.hpp"

#include <utility>

class Console;

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message routing service is a separate process, which routs messages
 *          to components. Applications, developed based on AREG SDK,
 *          should connect to message routing service to send and receive IPC messages.
 *          The business logic of message router to know logical relationship
 *          of running components and know where to redirect messages.
 *          Normally, for every connection channel type there should one instance of 
 *          message routing service also called as Multi-casting router (MCR).
 **/
class MulticastRouter final : public    SystemServiceBase
{
//////////////////////////////////////////////////////////////////////////
// internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   MulticastRouter::eRouterOptions
     *          The message router commands
     **/
    enum class eRouterOptions   : int32_t
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
     * \brief   Returns list of the options to validate contained in the pair object,
     *          where the first entry is the pointer to the list and second entry is
     *          the number of elements in the list
     **/
    static std::pair<const OptionParser::sOptionSetup *, int> getOptionSetup( void );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes instance of message router service.
     **/
    MulticastRouter( void );

    virtual ~MulticastRouter( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// SystemServiceBase overrides
/************************************************************************/

    /**
     * \brief   Called from main to start execution of  message router service.
     * \param   argc    The 'argc' parameter passed from 'main', indicates the number of parameters passed to executable.
     * \param   argv    The 'argv' parameter passed from 'main', indicated parameters passed to executable.
     **/
    virtual void serviceMain( int argc, char ** argv ) override;

    /**
     * \brief   Call to install (register) message router service in the system.
     * \return  Returns true if registration succeeded.
     **/
    virtual bool serviceInstall( void ) override;

    /**
     * \brief   Call to uninstall (unregister) message router service in the system.
     **/
    virtual void serviceUninstall( void ) override;

    /**
     * \brief   Registers system service in the system.
     **/
    virtual bool registerService( void ) override;

    /**
     * \brief   Opens operating system service DB for further processing.
     * \return  Returns true if succeeded.
     **/
    virtual bool serviceOpen( void ) override;

    /**
     * \brief   Called to start message router service.
     * \return  Returns true, if started with success.
     **/
    virtual bool serviceStart( void ) override;

    /**
     * \brief   Called to pause message router service.
     **/
    virtual void servicePause( void ) override;

    /**
     * \brief   Called to resume paused message router service.
     **/
    virtual bool serviceContinue( void ) override;

    /**
     * \brief   Called to stop message router service.
     **/
    virtual void serviceStop( void ) override;

    /**
     * \brief   Sets the state of message router service.
     **/
    virtual bool setState( NESystemService::eSystemServiceState newState ) override;

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

    /**
     * \brief   Run application as a background process without input or output on console. 
     **/
    virtual void runService(void) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the instance of the remote servicing object.
     **/
    inline IEServiceConnectionProvider & getService( void );

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
// OS specific hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    
    /**
     * \brief   OS specific validity check of message router service.
     **/
    bool _osIsValid( void ) const;

    /**
     * \brief   Called to free engaged resources.
     **/
    void _osFreeResources( void );

    /**
     * \brief   OS specific implementation to open service.
     **/
    bool _osOpenService( void );

    /**
     * \brief   OS specific implementation to create service.
     **/
    bool _osCcreateService( void );

    /**
     * \brief   OS specific implementation of deleting service.
     **/
    void _osDeleteService( void );

    /**
     * \brief   Registers service and returns true if handle is valid.
     *          The method is valid for Windows OS.
     **/
    bool _osRegisterService( void );

    /**
     * \brief   OS specific implementation of changing the state of the mcrouter service.
     **/
    bool _osSetState( NESystemService::eSystemServiceState newState );

    /**
     * \brief   OS specific implementation of waiting for user input on console.
     * \param   buffer  The allocated buffer to stream input from console.
     * \param   bufSize The size of allocated bugger.
     * \return  Returns true if succeeded to get user input.
     **/
    bool _osWaitUserInput(char* buffer, unsigned int bufSize);

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

inline IEServiceConnectionProvider & MulticastRouter::getService( void )
{
    return static_cast<IEServiceConnectionProvider &>(mServiceServer);
}

inline const NEService::MapInstances & MulticastRouter::getConnetedInstances( void ) const
{
    return mServiceServer.getInstances( );
}

inline MulticastRouter & MulticastRouter::self( void )
{
    return (*this);
}

#endif  // AREG_MCROUTER_APP_MULTICASTROUTER_HPP
