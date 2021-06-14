#ifndef AREG_EXAMPLES_13_IPCMIX_SHAREIPCMIX_MAINSERVICINGCOMPONENT_HPP
#define AREG_EXAMPLES_13_IPCMIX_SHAREIPCMIX_MAINSERVICINGCOMPONENT_HPP
/************************************************************************
 * \file        shareipcmix/src/MainServicingComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of remote servicing 
 *              component to output messages and shutdown the system.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "shareipcmix/src/RemoteServicingComponent.hpp"
#include "shareipcmix/src/SystemShutdownStub.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

class MainServicingComponent    : public    RemoteServicingComponent
                                , protected SystemShutdownStub
                                , private   IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    MainServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~MainServicingComponent(void);

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Call to register client. Each client should be registered before starting communication.
     * \param   name    The name of the client.
     * \param   service The service address of the client.
     * \param   thread  The thread name where client is running. Required to provide uniqueness.
     * \param   process The name of process. Optional parameter, used to make output in logs.
     * \see     responseRegister
     **/
    virtual void requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process );

    /**
     * \brief   Request call.
     *          Sent to unregister connected client.
     * \param   client  The client registration object indicating the unregistered client.
     * \note    Has no response
     **/
    virtual void requestUnregister( const NERemoteRegistry::sClientRegister & client );

    /**
     * \brief   Request call.
     *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
     * \param   clientID    The ID of registered client to make message output
     * \param   addMessage  The additional message to output. Ignored if empty.
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( unsigned int clientID, const String & addMessage );

/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder );

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client      The address of proxy client, which connection status is changed.
     * \param   isConnected Flag, indicating whether client is connected or disconnected.
     *                      When client disconnects, all listeners are removed.
     **/
    virtual void clientConnected( const ProxyAddress & client, bool isConnected );

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    Timer   mTimer; //!< The timer to trigger to wait until shutdown the application

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline MainServicingComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    MainServicingComponent( void );
    MainServicingComponent( const MainServicingComponent & /*src*/);
    const MainServicingComponent & operator = ( const MainServicingComponent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// MainServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline MainServicingComponent & MainServicingComponent::self( void )
{
    return (*this);
}

#endif // !AREG_EXAMPLES_13_IPCMIX_SHAREIPCMIX_MAINSERVICINGCOMPONENT_HPP
