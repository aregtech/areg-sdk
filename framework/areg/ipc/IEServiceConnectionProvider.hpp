#ifndef AREG_IPC_IESERVICECONNECTIONPROVIDER_HPP
#define AREG_IPC_IESERVICECONNECTIONPROVIDER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IEServiceConnectionProvider.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of Remote Service connection provider
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class String;

//////////////////////////////////////////////////////////////////////////
// IEServiceConnectionProvider interface
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is an interface of Remote Service Connection, which should be implemented
 *          on client and server side to handle remote service functionalities
 *          like configuring service, start and stop, registering and
 *          unregistering services and its clients.
 **/
class AREG_API IEServiceConnectionProvider
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor. Protected.
     **/
    IEServiceConnectionProvider( void ) = default;

    /**
     * \brief   Destructor
     **/
    virtual ~IEServiceConnectionProvider( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServiceConnectionProvider interface overrides
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
    virtual bool setupServiceConnectionHost( const String & configFile ) = 0;

    /**
     * \brief   Call manually to set router service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or host name of routing service to connect.
     * \param   portNr      Port number of routing service to connect.
     **/
    virtual void applyServiceConnectionData( const String & hostName, unsigned short portNr ) = 0;

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool connectServiceHost( void ) = 0;

    /**
     * \brief   Call to restart remove service. The host name and the port number should be already set.
     *          If the service had connection, it will be lost and re-connected again. If there was no
     *          connection, it starts new connection.
     * \return  Returns true if succeeded to restart service.
     **/
    virtual bool reconnectServiceHost(void) = 0;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void disconnectServiceHost( void ) = 0;

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    virtual bool isServiceHostConnected( void ) const = 0;

    /**
     * \brief   Returns true if service is configured and ready to start
     **/
    virtual bool isServiceHostSetup( void ) const = 0;

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

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEServiceConnectionProvider );
};

#endif  // AREG_IPC_IESERVICECONNECTIONPROVIDER_HPP
