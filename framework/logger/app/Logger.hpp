#ifndef AREG_LOGGER_APP_LOGGER_HPP
#define AREG_LOGGER_APP_LOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/app/Logger.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger to run as a console application process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "extend/service/SystemServiceBase.hpp"

#include "areg/base/SynchObjects.hpp"
#include "logger/app/NELoggerSettings.hpp"
#include "logger/service/LoggerServerService.hpp"

class Console;

//////////////////////////////////////////////////////////////////////////
// Logger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Logger service is a separate process, which receives
 *          and collects the log messages from the running applications.
 *          It may save logs in the file or forward to log viewer application..
 **/
class Logger    : public SystemServiceBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The commands to handle the logger.
     **/
    enum class eLogCommands : int32_t
    {
          CMD_LogUndefined  //!< Do nothing, should not happen
        , CMD_LogPause      //!< Pause logger.
        , CMD_LogRestart    //!< Restart logger.
        , CMD_LogSetScope   //!< Set the scope priorities
        , CMD_LogInstances  //!< Display the names of connected instances
        , CMD_LogSaveLogs   //!< Logger save logs in the file
        , CMD_LogSaveConfig //!< Save the log configuration
        , CMD_LogPrintHelp  //!< Output help message
        , CMD_LogQuit       //!< Quit logger
    };

    /**
     * \brief   The setup to validate input options of the logger.
     **/
    static const OptionParser::sOptionSetup ValidOptions[ ];

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of the Logger.
     **/
    static Logger& getInstance( void );

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
     * \brief   Default constructor and destructor.
     **/
    Logger( void );

    virtual ~Logger(void);

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
// SystemServiceBase overrides
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

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the instance of the remote servicing object.
     **/
    inline IEServiceConnectionProvider & getService( void );

    /**
     * \brief   Returns instance of the logger service.
     **/
    inline Logger & self( void );

    /**
     * \brief   Checks the command typed on console. Relevant only if it runs as a console application.
     * \param   cmd     The command typed on the console.
     * \return  Returns true if command is recognized. Otherwise, returns false.
     **/
    static bool _checkCommand(const String& cmd);

//////////////////////////////////////////////////////////////////////////
// OS specific hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   OS specific validity check of logger service.
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
    bool _osSetState(NESystemService::eSystemServiceState newState );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The service connection object to communicate with processes.
     **/
    LoggerServerService mServiceServer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Logger );
};

//////////////////////////////////////////////////////////////////////////
// Logger class inline methods.
//////////////////////////////////////////////////////////////////////////

inline IEServiceConnectionProvider & Logger::getService( void )
{
    return static_cast<IEServiceConnectionProvider &>(mServiceServer);
}

inline Logger & Logger::self( void )
{
    return (*this);
}

#endif  // AREG_LOGGER_APP_LOGGER_HPP
