#ifndef AREG_IPC_IEREMOTESERVICE_HPP
#define AREG_IPC_IEREMOTESERVICE_HPP
/************************************************************************
 * \file        areg/ipc/IERemoteService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Remote Service Interface
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEStubAddress;
class CEProxyAddress;
class CERemoteMessage;
class CEStreamableEvent;
class CEDispatcherThread;
class IERemoteEventConsumer;

//////////////////////////////////////////////////////////////////////////
// IERemoteService interface
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is an interface of Remote Service, which should be implemented
 *          on client and server side to handle remote service functionalities
 *          like configuring service, start and stop, registering and
 *          unregistering services and its clients.
 **/
class AREG_API IERemoteService
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor. Protected.
     **/
    IERemoteService( void );

    /**
     * \brief   Destructor
     **/
    virtual ~IERemoteService( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IERemoteService interface overrides
/************************************************************************/

    /**
     * \brief   Call to configure remote service. The passed file name
     *          can be either absolute or relative path.
     *          The function will read configuration file and initialize settings.
     *          If file path is NULL or empty, Remote Service will have default 
     *          configuration settings.
     * \param   configFile  Relative or absolute path of remote service configuration file.
     *                      If NULL or empty, it will use default settings.
     * \return  Returns true if system could configure. Otherwise, it returns false.
     **/
    virtual bool ServiceConfigure( const char * configFile ) = 0;

    /**
     * \brief   Call manually to set router service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or name of service broker host.
     * \param   portNr      Port number of service broker service.
     **/
    virtual void SetRouterServiceAddress( const char * hostName, unsigned short portNr ) = 0;

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool StartRemotingService( void ) = 0;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void StopRemotingService( void ) = 0;

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    virtual bool IsServiceStarted( void ) const = 0;

    /**
     * \brief   Returns true if service is configured and ready to start
     **/
    virtual bool IsServiceConfigured( void ) const = 0;

    /**
     * \brief   Returns true if remote service is enabled.
     **/
    virtual bool IsServiceEnabled( void ) const = 0;

    /**
     * \brief   Enables or disables remote service.
     *          The method should be implemented to set business logic of enabling and disabling
     *          remote service in case if it is already started.
     * \param   enable  If true, the service is enabled. Otherwise, it is disabled.
     **/
    virtual void EnableService( bool enable ) = 0;

    /**
     * \brief   Call to register remote service server stub object.
     *          All clients waiting for service should be connected notifications.
     * \param   stubService     The address of server stub service to register in system
     *                          The address contains service name and role name of service.
     * \return  Returns true if succeeded to start registration.
     **/
    virtual bool RegisterService( const CEStubAddress & stubService ) = 0;

    /**
     * \brief   Call to unregister previously registered server stub interface.
     * \param   stubService     The address of server stub service to unregister in system.
     **/
    virtual void UnregisterService( const CEStubAddress & stubService ) = 0;

    /**
     * \brief   Call to register client proxy of service. If system already has registered
     *          service server stub, the client will receive connected notification.
     *          Otherwise, the client will be in disconnected state as long, until server
     *          service is not registered in system.
     * \param   proxyService    The address of client proxy to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool RegisterServiceClient( const CEProxyAddress & proxyService ) = 0;

    /**
     * \brief   Call to unregister previously registered client prosy service.
     * \param   proxyService    The address of client proxy to unregister from system.
     **/
    virtual void UnregisterServiceClient( const CEProxyAddress & proxyService ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IERemoteService( const IERemoteService & /*src*/ );
    const IERemoteService & operator = ( const IERemoteService & /*src*/ );
};

#endif  // AREG_IPC_IEREMOTESERVICE_HPP
