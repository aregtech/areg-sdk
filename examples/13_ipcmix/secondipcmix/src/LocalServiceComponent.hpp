#ifndef AREG_EXAMPLES_13_IPCMIX_MAINIPCMIX_LOCALSERVICECOMPONENT_HPP
#define AREG_EXAMPLES_13_IPCMIX_MAINIPCMIX_LOCALSERVICECOMPONENT_HPP
/************************************************************************
 * \file        LocalServiceComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of local servicing interface
 *              and clients to output messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "shareipcmix/src/LocalServicingComponent.hpp"
#include "shareipcmix/src/RemoteServiceClient.hpp"
#include "shareipcmix/src/LocalServiceClient.hpp"

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
    static const unsigned int   REMOTE_TIMEOUT      /*= 450*/;  //!< The timeout to send request to remote service
    static const unsigned int   LOCAL_TIMEOUT       /*= 800*/;  //!< The timeout to send request to local service

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
    virtual ~LocalServiceComponent(void);

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
    LocalServiceComponent( void );
    LocalServiceComponent( const LocalServiceComponent & /*src*/);
    const LocalServiceComponent & operator = ( const LocalServiceComponent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// LocalServiceComponent inline methods.
//////////////////////////////////////////////////////////////////////////
inline LocalServiceComponent & LocalServiceComponent::self( void )
{
    return (*this);
}

#endif // AREG_EXAMPLES_13_IPCMIX_MAINIPCMIX_LOCALSERVICECOMPONENT_HPP

