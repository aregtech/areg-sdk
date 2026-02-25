#ifndef PUBMESH_COMMON_SRC_LOCALHELLOWORLDSERVICE_HPP
#define PUBMESH_COMMON_SRC_LOCALHELLOWORLDSERVICE_HPP

/************************************************************************
 * \file        common/src/LocalHelloWorldService.hpp
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
#include "examples/16_pubmesh/services/LocalHelloWorldStub.hpp"

//! \brief  A local service to receive request to output message on console
class LocalHelloWorldService  : private LocalHelloWorldStub
{

    //!< The type of list of connected clients.
    using ClientList = areg::LinkedList<LocalHelloWorld::sConnectedClient>;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   masterComp  The service owner component, required by LocalHelloWorldStub.
     **/
    LocalHelloWorldService( Component & masterComp );

    virtual ~LocalHelloWorldService() = default;

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
    void requestHelloWorld( const areg::String & roleName ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ClientList      mClientList;    //!< The list of connected clients.
    uint32_t    mNumRequests;   //!< The number of executed requests.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    LocalHelloWorldService() = delete;
    AREG_NOCOPY_NOMOVE( LocalHelloWorldService );
};

#endif  // PUBMESH_COMMON_SRC_LOCALHELLOWORLDSERVICE_HPP
