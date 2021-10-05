#pragma once

/************************************************************************
 * \file        generated/src/RemoteServiceClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/RemoteRegistryClientBase.hpp"
#include "generated/src/SystemShutdownClientBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
class RemoteServiceClient   : protected RemoteRegistryClientBase
                            , protected SystemShutdownClientBase
                            , private   IETimerConsumer
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
    RemoteServiceClient(const String & roleName, Component & owner, unsigned int timeout);

    /**
     * \brief   Destructor
     **/
    virtual ~RemoteServiceClient(void) = default;

protected:

    /**
     * \brief   Response callback.
     *          Sent when the client is registered. Each registered client has unique ID.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   client  The client registration object that contains unique ID.
     * \see     requestRegister
     **/
    virtual void responseRegister( const NERemoteRegistry::sClientRegister & client ) override;

    /**
     * \brief   Response callback.
     *          Called as reply to the request to output message.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientID    Indicates the ID client that made message output
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( unsigned int clientID ) override;

    /**
     * \brief   Server broadcast.
     *          Sent to notify the service unavailable state. All clients should be unregistered to start the shutdown procedure.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     **/
    virtual void broadcastServiceUnavailable( void ) override;

    /**
     * \brief   Triggered, when ServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceState description: 
     *          Describes the current state of service.
     * \param   ServiceState    The value of ServiceState attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onServiceStateUpdate( NESystemShutdown::eServiceState ServiceState, NEService::eDataStateType state ) override;

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
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline RemoteServiceClient & self( void );

    inline String timerName( Component & owner ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
protected:
    const unsigned int  mMsTimeout; //!< The timeout for timer to trigger message output of remote service
    Timer               mTimer;     //!< The timer to trigger to send request to output message
    NERemoteRegistry::sClientRegister   mClient;    //!< The ID given by service.

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteServiceClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( RemoteServiceClient );
};

inline RemoteServiceClient & RemoteServiceClient::self( void )
{
    return (*this);
}
