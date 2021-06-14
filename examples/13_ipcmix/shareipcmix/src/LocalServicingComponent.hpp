#ifndef AREG_EXAMPLES_13_IPCMIX_SHAREIPCMIX_LOCALSERVICINGCOMPONENT_HPP
#define AREG_EXAMPLES_13_IPCMIX_SHAREIPCMIX_LOCALSERVICINGCOMPONENT_HPP
/************************************************************************
 * \file        shareipcmix/src/LocalServicingComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "shareipcmix/src/LocalHelloWorldStub.hpp"

//////////////////////////////////////////////////////////////////////////
// LocalServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple local servicing component with requests, response, broadcasts and attributes.
 *          The service outputs Hello Worlds message until reaches maximum number.
 *          It demonstrates the various ways to use:
 *              a. Request with connected response;
 *              b. Request without response;
 *              c. Broadcasts with and without parameters;
 *              d. Attribute with automatic and manual update notification
 **/
class LocalServicingComponent   : public    Component
                                , protected LocalHelloWorldStub
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
    LocalServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalServicingComponent(void);

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   roleName    The role name of client component that requested to print hello world
     * \param   addMessage  Additional message to output. Can be empty.
     *          Has default value: ""
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( const String & roleName, const String & addMessage = "" );

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
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    unsigned int    mGnerateID; //!< The client ID generator

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline LocalServicingComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    LocalServicingComponent( void );
    LocalServicingComponent( const LocalServicingComponent & /*src*/);
    const LocalServicingComponent & operator = ( const LocalServicingComponent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// LocalServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline LocalServicingComponent & LocalServicingComponent::self( void )
{   return (*this);     }


#endif // AREG_EXAMPLES_13_IPCMIX_SHAREIPCMIX_LOCALSERVICINGCOMPONENT_HPP
