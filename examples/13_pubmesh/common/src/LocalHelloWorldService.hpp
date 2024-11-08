#ifndef PUBMESH_COMMON_SRC_LOCALHELLOWORLDSERVICE_HPP
#define PUBMESH_COMMON_SRC_LOCALHELLOWORLDSERVICE_HPP

/************************************************************************
 * \file        common/src/LocalHelloWorldService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "examples/13_pubmesh/services/LocalHelloWorldStub.hpp"

//! \brief  A local service to receive request to output message on console
class LocalHelloWorldService  : private LocalHelloWorldStub
{

    //!< The type of list of connected clients.
    using ClientList = TELinkedList<NELocalHelloWorld::sConnectedClient>;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   masterComp  The service owner component, required by LocalHelloWorldStub.
     **/
    LocalHelloWorldService( Component & masterComp );

    virtual ~LocalHelloWorldService(void) = default;

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

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ClientList      mClientList;    //!< The list of connected clients.
    unsigned int    mNumRequests;   //!< The number of executed requests.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    LocalHelloWorldService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalHelloWorldService );
};

#endif  // PUBMESH_COMMON_SRC_LOCALHELLOWORLDSERVICE_HPP
