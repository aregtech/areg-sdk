#ifndef AREG_EXAMPLES_13_PUBSVCMIX_IPCMIXSECOND_SRC_REMOTESERVICECOMPONENT_HPP
#define AREG_EXAMPLES_13_PUBSVCMIX_IPCMIXSECOND_SRC_REMOTESERVICECOMPONENT_HPP
/************************************************************************
 * \file        ipcmixsecond/src/RemoteServiceComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/RemoteServicingComponent.hpp"

#include "generated/src/MainRemoteServiceClient.hpp"
#include "generated/src/RemoteServiceClient.hpp"
#include "generated/src/LocalServiceClient.hpp"

//////////////////////////////////////////////////////////////////////////
// RemoteServiceComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Secondary remote service component. It is similar to main
 *          servicing component, except that it will not trigger shutdown.
 **/
class RemoteServiceComponent  : public RemoteServicingComponent
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants
//////////////////////////////////////////////////////////////////////////
    static const unsigned int   MAIN_MESSAGE_TIMEOUT    = 1200; //!< The timeout to send message to remote main servicing component
    static const unsigned int   REMOTE_MESSAGE_TIMEOUT  =  800; //!< The timeout to send request to some other remote servicing component instantiated in the same thread.
    static const unsigned int   LOCAL_MESSAGE_TIMEOUT   =  500; //!< The timeout to communicate with local servicing objects.

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
    RemoteServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteServiceComponent(void);

private:

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    MainRemoteServiceClient mMain;      //!< Client of remote main service
    RemoteServiceClient     mRemote;    //!< Client of some other remote service.
    LocalServiceClient      mLocal2;    //!< Client of local service
    LocalServiceClient      mLocal3;    //!< Client of local service 

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////

    inline RemoteServiceComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    RemoteServiceComponent( void );
    RemoteServiceComponent( const RemoteServiceComponent & /*src*/);
    const RemoteServiceComponent & operator = ( const RemoteServiceComponent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteServiceComponent inline method implementation
//////////////////////////////////////////////////////////////////////////

inline RemoteServiceComponent & RemoteServiceComponent::self( void )
{
    return (*this);
}

#endif // AREG_EXAMPLES_13_PUBSVCMIX_IPCMIXSECOND_SRC_REMOTESERVICECOMPONENT_HPP
