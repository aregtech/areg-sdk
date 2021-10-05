#pragma once

/************************************************************************
 * \file        generated/src/RemoteServicingComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of remote servicing 
 *              component to output messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/RemoteRegistryStub.hpp"

//////////////////////////////////////////////////////////////////////////
// RemoteServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple remote servicing component with requests, response, broadcasts and attributes.
 *          The service outputs Hello Worlds message until reaches maximum number.
 *          It demonstrates the various ways to use:
 *              a. Request with connected response;
 *              b. Request without response;
 *              c. Broadcasts with and without parameters;
 *              d. Attribute with automatic and manual update notification
 **/
class RemoteServicingComponent  : public    Component
                                , protected RemoteRegistryStub
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
    RemoteServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteServicingComponent(void) = default;

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
    virtual void requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process ) override;

    /**
     * \brief   Request call.
     *          Sent to unregister connected client.
     * \param   client  The client registration object indicating the unregistered client.
     * \note    Has no response
     **/
    virtual void requestUnregister( const NERemoteRegistry::sClientRegister & client ) override;

    /**
     * \brief   Request call.
     *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
     * \param   clientID    The ID of registered client to make message output
     * \param   addMessage  The additional message to output. Ignored if empty.
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( unsigned int clientID, const String & addMessage ) override;

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
    virtual void startupServiceInterface( Component & holder ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    unsigned int    mGnerateID; //!< The client ID generator

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline RemoteServicingComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    RemoteServicingComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( RemoteServicingComponent );
};

//////////////////////////////////////////////////////////////////////////
// RemoteServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline RemoteServicingComponent & RemoteServicingComponent::self( void )
{
    return (*this);
}
