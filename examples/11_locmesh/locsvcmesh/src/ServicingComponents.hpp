#pragma once

/************************************************************************
 * \file        ServicingComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloWorldStub.hpp"

#include "locsvcmesh/src/ServiceClient.hpp"

//////////////////////////////////////////////////////////////////////////
// ControllerComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple service with requests, response and broadcast.
 *          It demonstrates the use:
 *              a. Request with connected response;
 *              b. Request without response;
 *              c. Broadcasts with a parameter;
 **/
class HelloWorldService : protected HelloWorldStub
{
    //!< The type of list of connected clients.
    using ClientList = TELinkedList<NEHelloWorld::sConnectedClient>;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   masterComp  The master / owner component of the service.
     * \param   isMain      The flag indicating whether it is a main controller
     *                      that can trigger application shutdown or not.
     **/
    HelloWorldService( Component & masterComp, bool isMain );

    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWorldService(void) = default;

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   roleName    The role name of client component that requested to print hello world
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( const String & roleName ) override;

    /**
     * \brief   Request call.
     *          Sent by client to notify the shutdown. This removes client from the list. This request has no response.
     * \param   clientID    The ID of client that requests to shutdown. The ID is given by service when first time client requests to output message.
     * \param   roleName    Service client component role name
     * \note    Has no response
     **/
    virtual void requestClientShutdown( unsigned int clientID, const String & roleName ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    const bool      mIsMain;        //!< Flag, indicating whether it is a main controller service or not.
    unsigned int    mGnerateID;     //!< The client ID generator
    ClientList      mClientList;    //!< The list of connected clients.
    signed short    mRemainRequest; //!< The maximum number of processing requests.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    HelloWorldService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldService );
};

//////////////////////////////////////////////////////////////////////////
// ControllerComponent class declaration
//////////////////////////////////////////////////////////////////////////
//! \brief   The servicing component, which controls the application shutdown.
class ControllerComponent   : public    Component
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ControllerComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ControllerComponent & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    HelloWorldService   mService;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ControllerComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ControllerComponent );
};

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent class declaration
//////////////////////////////////////////////////////////////////////////
class SecondaryComponent    : public    Component
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    SecondaryComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

private:
//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////

    inline SecondaryComponent & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    HelloWorldService   mService;           //!< The service included in the component
    ServiceClient       mClientMain;        //!< Service client of main component
    ServiceClient       mClientSecondary;   //!< Service client of secondary component.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SecondaryComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SecondaryComponent );
};
