#pragma once

/************************************************************************
 * \file        locservice/src/ServicingComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "examples/13_locsvc/services/HelloWorldStub.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
//! \brief   A demo of servicing component with requests, response and broadcasts.
class ServicingComponent    : public    Component
                            , protected HelloWorldStub
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

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
     *          Sent by client to request to shutdown service and the application. This request has no response.
     * \note    Has no response
     **/
    virtual void requestShutdownService( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    signed short    mRemainRequest; //!< The maximum number of processing requests.

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ServicingComponent & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServicingComponent );
};
