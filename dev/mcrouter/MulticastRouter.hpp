#ifndef MCROUTER_MULTICASTROUTER_HPP
#define MCROUTER_MULTICASTROUTER_HPP
/************************************************************************
 * \file        mcrouter/MulticastRouter.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Multicast routing to run as process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "mcrouter/NEMulticastRouterSettings.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/ipc/ServerService.hpp"

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message broker is a separate process, which routs messages
 *          to components. Applications, developed based on AREG SDK,
 *          should connect to message broker to send and receive IPC messages.
 *          The business logic of message broker to know logical relationship
 *          of running components and know where to redirect messages.
 *          Normally, for every connection channel type there should one message broker.
 **/
class MulticastRouter
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Message broker Servicing commands
     **/
    typedef enum E_ServiceCommand
    {
          CMD_Undefined     //!< Command is undefined
        , CMD_Install       //!< Command is to install (register) service in the system
        , CMD_Uninstall     //!< Command is to uninstall (unregister) service in the system
        , CMD_Service       //!< Command is to execute process as a system service (in background)
        , CMD_Console       //!< Command is to execute process as console application.
    } eServiceCommand;

    /**
     * \brief   Converts and returns string value of MulticastRouter::eServiceCommand type.
     **/
    static inline const char * getString( MulticastRouter::eServiceCommand cmdService );

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of message broker.
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
     * \brief   Call to un-install (unregister) message router service in the system.
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
    bool serviceOpen( void );

    /**
     * \brief   Returns current command of message router service.
     **/
    MulticastRouter::eServiceCommand getCurrentCommand( void ) const;

    /**
     * \brief   Sets the current command of message router service.
     * \param   cmdService  The message router service command to set.
     **/
    inline void setCurrentCommand( MulticastRouter::eServiceCommand cmdService );

    /**
     * \brief   Returns the state of message router service.
     **/
    NEMulticastRouterSettings::eRouterState getState( void ) const;

    /**
     * \brief   Sets the state of message router service.
     **/
    bool setState( NEMulticastRouterSettings::eRouterState newState );

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Called to free engaged resources.
     **/
    void _freeResources( void );
    /**
     * \brief   OS specific implementation to open service.
     **/
    bool _openService( void );
    /**
     * \brief   OS specific implementation to create service.
     **/
    bool _createService( void );
    /**
     * \brief   OS specific validity check of message router service.
     **/
    bool _isValid( void ) const;
    /**
     * \brief   Returns instance of message router service.
     **/
    inline MulticastRouter & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The message router service state.
     **/
    NEMulticastRouterSettings::eRouterState  mRouterState;
    /**
     * \brief   The current command to execute by message router service.
     **/
    MulticastRouter::eServiceCommand       mServiceCmd;
    /**
     * \brief   The special event to stop and quit message router service.
     **/
    SynchEvent      mQuitEvent;
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
    MulticastRouter( const MulticastRouter & /*src*/ );
    const MulticastRouter & operator = (const MulticastRouter & /*src*/);
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
    return _openService();
}

inline MulticastRouter::eServiceCommand MulticastRouter::getCurrentCommand(void) const
{
    return mServiceCmd;
}

inline void MulticastRouter::setCurrentCommand(MulticastRouter::eServiceCommand cmdService)
{
    mServiceCmd = cmdService;
}

inline MulticastRouter & MulticastRouter::self( void )
{
    return (*this);
}

inline const char * MulticastRouter::getString(MulticastRouter::eServiceCommand cmdService)
{
    switch ( cmdService )
    {
    case MulticastRouter::CMD_Undefined:
        return "MulticastRouter::CMD_Undefined";
    case MulticastRouter::CMD_Install:
        return "MulticastRouter::CMD_Install";
    case MulticastRouter::CMD_Uninstall:
        return "MulticastRouter::CMD_Uninstall";
    case MulticastRouter::CMD_Service:
        return "MulticastRouter::CMD_Service";
    case MulticastRouter::CMD_Console:
        return "MulticastRouter::CMD_Console";
    default:
        ASSERT(false);
        return "ERR: Unexpected MulticastRouter::eServiceCommand value!";
    }
}

#endif  // MCROUTER_MulticastRouter_HPP
