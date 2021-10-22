#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IERemoteService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Remote Service Interface
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class StubAddress;
class ProxyAddress;
class RemoteMessage;
class StreamableEvent;
class DispatcherThread;
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
    IERemoteService( void ) = default;

    /**
     * \brief   Destructor
     **/
    virtual ~IERemoteService( void ) = default;

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
     *          If file path is nullptr or empty, Remote Service will have default 
     *          configuration settings.
     * \param   configFile  Relative or absolute path of remote service configuration file.
     *                      If nullptr or empty, it will use default settings.
     * \return  Returns true if system could configure. Otherwise, it returns false.
     **/
    virtual bool configureRemoteServicing( const char * configFile ) = 0;

    /**
     * \brief   Call manually to set router service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or host name of routing service to connect.
     * \param   portNr      Port number of routing service to connect.
     **/
    virtual void setRemoteServiceAddress( const char * hostName, unsigned short portNr ) = 0;

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool startRemoteServicing( void ) = 0;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void stopRemoteServicing( void ) = 0;

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    virtual bool isRemoteServicingStarted( void ) const = 0;

    /**
     * \brief   Returns true if service is configured and ready to start
     **/
    virtual bool isRemoteServicingConfigured( void ) const = 0;

    /**
     * \brief   Returns true if remote service is enabled.
     **/
    virtual bool isRemoteServicingEnabled( void ) const = 0;

    /**
     * \brief   Enables or disables remote service.
     *          The method should be implemented to set business logic of enabling and disabling
     *          remote service in case if it is already started.
     * \param   enable  If true, the service is enabled. Otherwise, it is disabled.
     **/
    virtual void enableRemoteServicing( bool enable ) = 0;

    /**
     * \brief   Call to register remote service server stub object.
     *          All clients waiting for service should be connected notifications.
     * \param   stubService     The address of server stub service to register in system
     *                          The address contains service name and role name of service.
     * \return  Returns true if succeeded to start registration.
     **/
    virtual bool registerService( const StubAddress & stubService ) = 0;

    /**
     * \brief   Call to unregister previously registered server stub interface.
     * \param   stubService     The address of server stub service to unregister in system.
     **/
    virtual void unregisterService( const StubAddress & stubService ) = 0;

    /**
     * \brief   Call to register client proxy of service. If system already has registered
     *          service server stub, the client will receive connected notification.
     *          Otherwise, the client will be in disconnected state as long, until server
     *          service is not registered in system.
     * \param   proxyService    The address of client proxy to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool registerServiceClient( const ProxyAddress & proxyService ) = 0;

    /**
     * \brief   Call to unregister previously registered client prosy service.
     * \param   proxyService    The address of client proxy to unregister from system.
     **/
    virtual void unregisterServiceClient( const ProxyAddress & proxyService ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IERemoteService );
};
