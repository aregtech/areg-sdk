#pragma once

/************************************************************************
 * \file        pubservice/src/PublicServiceComponent.hpp
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
#include "generated/src/SystemShutdownStub.hpp"
#include "common/src/PublicHelloWorldService.hpp"
#include "common/src/LocalHelloWorldClient.hpp"

//!<\ brief     The public service component, which controls
//!             the service start and shutdown states.
class PublicServiceComponent    : public    Component
                                , private   SystemShutdownStub
                                , private   PublicHelloWorldService
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants.
//////////////////////////////////////////////////////////////////////////
    static constexpr unsigned  int  LOCAL_TIMEOUT   { 555 };//!< Timeout to output message in local service.

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
private:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    PublicServiceComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    virtual ~PublicServiceComponent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void startupComponent( ComponentThread & comThread ) override;

    /**
     * \brief   Request call.
     *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
     * \param   clientID    The ID of registered client to make message output
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( unsigned int clientID ) override;

    /**
     * \brief   Request call.
     *          The request to shutdown the system.
     * \note    Has no response
     **/
    virtual void requestSystemShutdown( void ) override;

private:
//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    LocalHelloWorldClient   mLocalClient;   //! Client of local service

    inline PublicServiceComponent & self( void )
    {
        return (*this);
    }


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    PublicServiceComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PublicServiceComponent );
};
