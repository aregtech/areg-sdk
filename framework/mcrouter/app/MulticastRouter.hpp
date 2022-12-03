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
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/MulticastRouter.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing to run as process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "mcrouter/app/NEMulticastRouterSettings.hpp"
#include "mcrouter/tcp/ServerService.hpp"
#include "areg/base/SynchObjects.hpp"

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
class MulticastRouter
{
//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of multi-cast router (MCR).
     **/
    static MulticastRouter & getInstance( void );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes instance of message router service.
     **/
    MulticastRouter( void );
    /**
     * \brief   Destructor.
     **/
    virtual ~MulticastRouter( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to install (register) message router service in the system.
     * \return  Returns true if registration succeeded.
     **/
    bool serviceInstall( void );

    /**
     * \brief   Call to uninstall (unregister) message router service in the system.
     **/
    void serviceUninstall( void );

    /**
     * \brief   Called from main to start execution of  message router service.
     * \param   argc    The 'argc' parameter passed from 'main', indicates the number of parameters passed to executable.
     * \param   argv    The 'argv' parameter passed from 'main', indicated parameters passed to executable.
     **/
    void serviceMain( int argc, char ** argv );

    /**
     * \brief   Called to start message router service.
     * \return  Returns true, if started with success.
     **/
    bool serviceStart( void );

    /**
     * \brief   Called to pause message router service.
     **/
    void servicePause( void );

    /**
     * \brief   Called to resume paused message router service.
     **/
    bool serviceContinue( void );

    /**
     * \brief   Called to stop message router service.
     **/
    void serviceStop( void );

    /**
     * \brief   Opens operating system service DB for further processing.
     * \return  Returns true if succeeded.
     **/
    inline bool serviceOpen( void );

    /**
     * \brief   Returns current command of message router service.
     **/
    inline NEMulticastRouterSettings::eServiceCommand getCurrentCommand( void ) const;

    /**
     * \brief   Sets the current command of message router service.
     * \param   cmdService  The message router service command to set.
     **/
    inline void setCurrentCommand( NEMulticastRouterSettings::eServiceCommand cmdService );

    /**
     * \brief   Returns the state of message router service.
     **/
    inline NEMulticastRouterSettings::eRouterState getState( void ) const;

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

    inline bool isVerbose( void ) const;

    /**
     * \brief   Sets the state of message router service.
     **/
    bool setState( NEMulticastRouterSettings::eRouterState newState );

    /**
     * \brief   Parses the options and returns true if succeeded.
     * \param   argc    The number of options to parse.
     * \param   argv    The options to parse.
     */
    bool parseOptions(int argc, char** argv);

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
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
    bool _osSetState( NEMulticastRouterSettings::eRouterState newState );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The message router service state.
     **/
    NEMulticastRouterSettings::eRouterState     mRouterState;
    /**
     * \brief   The current command to execute by message router service.
     **/
    NEMulticastRouterSettings::eServiceCommand  mServiceCmd;
    /**
     * \brief   Flag, indicating whether the process should run verbose or not. Valid only if process runs as console application.
     */
    bool            mRunVerbose;
    /**
     * \brief   The instance of message router service server to accept connections from applications.
     **/
    ServerService   mServiceServer;
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
    DECLARE_NOCOPY_NOMOVE( MulticastRouter );
};

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class inline methods.
//////////////////////////////////////////////////////////////////////////

inline NEMulticastRouterSettings::eRouterState MulticastRouter::getState( void ) const
{
    return mRouterState;
}

inline bool MulticastRouter::serviceOpen(void)
{
    return _osOpenService();
}

inline NEMulticastRouterSettings::eServiceCommand MulticastRouter::getCurrentCommand(void) const
{
    return mServiceCmd;
}

inline void MulticastRouter::setCurrentCommand( NEMulticastRouterSettings::eServiceCommand cmdService)
{
    mServiceCmd = cmdService;
}

inline void MulticastRouter::resetDefaultOptions(void)
{
    mServiceCmd = NEMulticastRouterSettings::DEFAULT_OPTION;
    mRunVerbose = NEMulticastRouterSettings::DEFAULT_VERBOSE;
}

inline uint32_t MulticastRouter::queryDataReceived(void)
{
    return mServiceServer.queryBytesReceived();
}

inline uint32_t MulticastRouter::queryDataSent(void)
{
    return mServiceServer.queryBytesSent();
}

inline bool MulticastRouter::isVerbose(void) const
{
    return mRunVerbose;
}

inline MulticastRouter & MulticastRouter::self( void )
{
    return (*this);
}

#endif  // AREG_MCROUTER_APP_MULTICASTROUTER_HPP
