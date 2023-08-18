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
#include "logger/app/NELoggerSettings.hpp"
#include "areg/base/SynchObjects.hpp"

class Console;

//////////////////////////////////////////////////////////////////////////
// Logger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Logger service is a separate process, which receives
 *          and collects the log messages from the running applications.
 *          It may save logs in the file or forward to log viewer application..
 **/
class Logger
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The commands to handle the logger.
     **/
    enum eLogCommands : int32_t
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

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor and destructor.
     **/
    Logger( void );
    ~Logger(void);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to install (register) logger service in the system.
     * \return  Returns true if registration succeeded.
     **/
    bool serviceInstall( void );

    /**
     * \brief   Call to uninstall (unregister) logger service in the system.
     **/
    void serviceUninstall( void );

    /**
     * \brief   Called from main to start execution of logger service.
     * \param   argc    The 'argc' parameter passed from 'main', indicates the number of parameters passed to executable.
     * \param   argv    The 'argv' parameter passed from 'main', indicated parameters passed to executable.
     **/
    void serviceMain( int argc, char ** argv );

    /**
     * \brief   Called to start logger service.
     * \return  Returns true, if started with success.
     **/
    bool serviceStart( void );

    /**
     * \brief   Called to pause logger service.
     **/
    void servicePause( void );

    /**
     * \brief   Called to resume paused logger  service.
     **/
    bool serviceContinue( void );

    /**
     * \brief   Called to stop logger  service.
     **/
    void serviceStop( void );

    /**
     * \brief   Opens operating system service DB for further processing.
     * \return  Returns true if succeeded.
     **/
    inline bool serviceOpen( void );

    /**
     * \brief   Returns current command of the logger service.
     **/
    inline NELoggerSettings::eServiceCommand getCurrentCommand( void ) const;

    /**
     * \brief   Sets the current command of the logger service.
     * \param   cmdService  The logger  service command to set.
     **/
    inline void setCurrentCommand(NELoggerSettings::eServiceCommand cmdService );

    /**
     * \brief   Returns the state of logger service.
     **/
    inline NELoggerSettings::eLoggerState getState( void ) const;

    /**
     * \brief   Resets default options.
     **/
    inline void resetDefaultOptions(void);

    /**
     * \brief   Call to query the size in bytes of data sent.
     **/
    inline uint32_t queryDataReceived(void);

    /**
     * \brief   Call to query the size in bytes of data received.
     **/
    inline uint32_t queryDataSent(void);

    /**
     * \brief   Returns true if verbose flag is set.
     *          If verbose flag is set, it outputs the data rate in console.
     **/
    inline bool isVerbose( void ) const;

    /**
     * \brief   Sets the state of logger  service.
     **/
    bool setState(NELoggerSettings::eLoggerState newState );

    /**
     * \brief   Parses the options and returns true if succeeded.
     * \param   argc    The number of options to parse.
     * \param   argv    The options to parse.
     */
    bool parseOptions(int argc, const char** argv);

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
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
    bool _osSetState(NELoggerSettings::eLoggerState newState );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The logger service state.
     **/
    NELoggerSettings::eLoggerState      mLoggerState;
    /**
     * \brief   The current command to execute by logger service.
     **/
    NELoggerSettings::eServiceCommand   mServiceCmd;
    /**
     * \brief   Flag, indicating whether the process should run verbose or not. Valid only if process runs as console application.
     */
    bool            mRunVerbose;
    /**
     * \brief   OS specific service handle
     **/
    void *          mSvcHandle;
    /**
     * \brief   OS specific service manager handle.
     **/
    void *          mSeMHandle;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Logger );
};

//////////////////////////////////////////////////////////////////////////
// Logger class inline methods.
//////////////////////////////////////////////////////////////////////////

inline NELoggerSettings::eLoggerState Logger::getState( void ) const
{
    return mLoggerState;
}

inline bool Logger::serviceOpen(void)
{
    return _osOpenService();
}

inline NELoggerSettings::eServiceCommand Logger::getCurrentCommand(void) const
{
    return mServiceCmd;
}

inline void Logger::setCurrentCommand( NELoggerSettings::eServiceCommand cmdService)
{
    mServiceCmd = cmdService;
}

inline void Logger::resetDefaultOptions(void)
{
    mServiceCmd = NELoggerSettings::DEFAULT_OPTION;
    mRunVerbose = NELoggerSettings::DEFAULT_VERBOSE;
}

inline uint32_t Logger::queryDataReceived(void)
{
    return 0; // mServiceServer.queryBytesReceived();
}

inline uint32_t Logger::queryDataSent(void)
{
    return 0; // mServiceServer.queryBytesSent();
}

inline bool Logger::isVerbose(void) const
{
    return mRunVerbose;
}

inline Logger & Logger::self( void )
{
    return (*this);
}

#endif  // AREG_LOGGER_APP_LOGGER_HPP
