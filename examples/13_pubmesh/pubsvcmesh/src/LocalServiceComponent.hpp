#pragma once

/************************************************************************
 * \file        pubsvcmesh/src/LocalServiceComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of local servicing interface
 *              and clients to output messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/LocalServicingComponent.hpp"
#include "generated/src/RemoteServiceClient.hpp"
#include "generated/src/LocalServiceClient.hpp"

//////////////////////////////////////////////////////////////////////////
// LocalServiceComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A local service component. It also contains remote and 
 *          local service client object.
 **/
class LocalServiceComponent  : public LocalServicingComponent
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants
//////////////////////////////////////////////////////////////////////////
    static const unsigned int   REMOTE_TIMEOUT      = 1000; //!< The timeout to send request to remote service
    static const unsigned int   LOCAL_TIMEOUT       =  500; //!< The timeout to send request to local service

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

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    
    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    LocalServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalServiceComponent(void) = default;

private:
//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////

    RemoteServiceClient     mRemote;    //!< The client of remote servicing object
    LocalServiceClient      mLocal;     //!< The client of local servicing object

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////

    inline LocalServiceComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    LocalServiceComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalServiceComponent );
};

//////////////////////////////////////////////////////////////////////////
// LocalServiceComponent inline methods.
//////////////////////////////////////////////////////////////////////////
inline LocalServiceComponent & LocalServiceComponent::self( void )
{
    return (*this);
}
