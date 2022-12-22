#pragma once

/************************************************************************
 * \file        locsvcmesh/src/ServiceHelloWorld.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains implementation of a service.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/HelloWorldStub.hpp"

//! \brief  An implementation of a service to include in the components.
//!         This service receives requests, send response and broadcast.
class ServiceHelloWorld : protected HelloWorldStub
{
    //!< The type of list of connected clients.
    using ClientList = TEMap<String, unsigned int>;

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
    ServiceHelloWorld( Component & masterComp, bool isMain );

    /**
     * \brief   Destructor.
     **/
    virtual ~ServiceHelloWorld( void ) = default;

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
     * \param   clientID    The ID of client that requests to shutdown. The ID is given by service when first time client requests to output message.
     * \param   roleName    Service client component role name
     * \note    Has no response
     **/
    virtual void requestShutdownService( unsigned int clientID, const String & roleName ) override;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
private:
    const bool      mIsMain;        //!< Flag, indicating whether it is a main controller service or not.
    ClientList      mClientList;    //!< The list of connected clients.
    signed short    mRemainRequest; //!< The maximum number of processing requests.

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    ServiceHelloWorld( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceHelloWorld );
};
