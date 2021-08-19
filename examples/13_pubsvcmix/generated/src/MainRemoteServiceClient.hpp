#ifndef AREG_EXAMPLES_13_PUBSVCMIX_GENERATED_SRC_MAINREMOTESERVICECLIENT_HPP
#define AREG_EXAMPLES_13_PUBSVCMIX_GENERATED_SRC_MAINREMOTESERVICECLIENT_HPP
/************************************************************************
 * \file        generated/src/MainRemoteServiceClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/RemoteServiceClient.hpp"

/**
 * \brief   The client object of main service, which should initiate application quit
 *          when the main service is triggers system shutdown message.
 **/
class MainRemoteServiceClient   : public RemoteServiceClient
{

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \param   timeout The timeout in milliseconds to trigger message output on remote service.
     **/
    MainRemoteServiceClient(const String & roleName, Component & owner, unsigned int timeout);

    /**
     * \brief   Destructor
     **/
    virtual ~MainRemoteServiceClient(void);

protected:
    /**
     * \brief   Server broadcast.
     *          Notifies the system is shutting down so that application should disconnect and close.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     **/
    virtual void broadcastServiceShutdown( void );

/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

};

#endif // AREG_EXAMPLES_13_PUBSVCMIX_GENERATED_SRC_MAINREMOTESERVICECLIENT_HPP
