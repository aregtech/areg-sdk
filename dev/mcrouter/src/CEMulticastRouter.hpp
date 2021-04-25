#ifndef MCROUTER_CEMULTICASTROUTER_HPP
#define MCROUTER_CEMULTICASTROUTER_HPP
/************************************************************************
 * \file        mcrouter/src/CEMulticastRouter.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Multicast routing to run as process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "mcrouter/src/NEMulticastRouterSettings.hpp"
#include "areg/src/base/ESynchObjects.hpp"
#include "areg/src/ipc/CEServerService.hpp"

//////////////////////////////////////////////////////////////////////////
// CEMulticastRouter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message broker is a separate process, which routs messages
 *          to components. Applications, developed based on AREG SDK,
 *          should connect to message broker to send and receive IPC messages.
 *          The business logic of message broker to know logical relationship
 *          of running components and know where to redirect messages.
 *          Normally, for every connection channel type there should one message broker.
 **/
class CEMulticastRouter
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
     * \brief   Converts and returns string value of CEMulticastRouter::eServiceCommand type.
     **/
    static inline const char * GetString( CEMulticastRouter::eServiceCommand cmdService );

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of message broker.
     **/
    static CEMulticastRouter & GetMulticastRouter( void );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes message broker.
     **/
    CEMulticastRouter( void );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEMulticastRouter( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to install (register) message router service in the system.
     * \return  Returns true if registration succeeded.
     **/
    bool ServiceInstall( void );

    /**
     * \brief   Call to un-install (unregister) message router service in the system.
     **/
    void ServiceUninstall( void );

    void ServiceMain( int argc, char ** argv );

    bool ServiceStart( void );

    void ServicePause( void );

    bool ServiceContinue( void );

    void ServiceStop( void );

    bool ServiceOpen( void );

    CEMulticastRouter::eServiceCommand GetCurrentCommand( void ) const;

    inline void SetCurrentCommand( CEMulticastRouter::eServiceCommand cmdService );

public:
    NEMulticastRouterSettings::eRouterState GetState( void ) const;

    bool SetState( NEMulticastRouterSettings::eRouterState newState );

private:
    void _freeResources( void );
    bool _openService( void );
    bool _createService( void );
    bool _isValid( void ) const;

    inline CEMulticastRouter & self( void );

private:
    NEMulticastRouterSettings::eRouterState  mRouterState;

    CEMulticastRouter::eServiceCommand       mServiceCmd;

    CESynchEvent                    mQuitEvent;

    CEServerService                 mServiceServer;

    void *                          mSvcHandle;

    void *                          mSeMHandle;
};

inline NEMulticastRouterSettings::eRouterState CEMulticastRouter::GetState( void ) const
{
    return mRouterState;
}

inline bool CEMulticastRouter::ServiceOpen(void)
{
    return _openService();
}

inline CEMulticastRouter::eServiceCommand CEMulticastRouter::GetCurrentCommand(void) const
{
    return mServiceCmd;
}

inline void CEMulticastRouter::SetCurrentCommand(CEMulticastRouter::eServiceCommand cmdService)
{
    mServiceCmd = cmdService;
}

inline const char * CEMulticastRouter::GetString(CEMulticastRouter::eServiceCommand cmdService)
{
    switch ( cmdService )
    {
    case CEMulticastRouter::CMD_Undefined:
        return "CEMulticastRouter::CMD_Undefined";
    case CEMulticastRouter::CMD_Install:
        return "CEMulticastRouter::CMD_Install";
    case CEMulticastRouter::CMD_Uninstall:
        return "CEMulticastRouter::CMD_Uninstall";
    case CEMulticastRouter::CMD_Service:
        return "CEMulticastRouter::CMD_Service";
    case CEMulticastRouter::CMD_Console:
        return "CEMulticastRouter::CMD_Console";
    default:
        ASSERT(false);
        return "ERR: Unexpected CEMulticastRouter::eServiceCommand value!";
    }
}

inline CEMulticastRouter & CEMulticastRouter::self( void )
{
    return (*this);
}

#endif  // MCROUTER_CEMulticastRouter_HPP
