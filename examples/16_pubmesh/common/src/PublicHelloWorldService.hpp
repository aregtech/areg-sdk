#ifndef PUBMESH_COMMON_SRC_PUBLICHELLOWORLDSERVICE_HPP
#define PUBMESH_COMMON_SRC_PUBLICHELLOWORLDSERVICE_HPP

/************************************************************************
 * \file        common/src/PublicHelloWorldService.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of remote servicing 
 *              component to output messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "examples/16_pubmesh/services/PublicHelloWorldStub.hpp"

//! \brief  Implementation of a public service to receive requests from remote clients.
class PublicHelloWorldService : private PublicHelloWorldStub
{
    using ClientList = LinkedList< PublicHelloWorld::sClientRegister >;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of Component must be already initialized.
     **/
    PublicHelloWorldService(Component & masterComp);

    /**
     * \brief   Destructor.
     **/
    virtual ~PublicHelloWorldService() = default;

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Call to register client. Each client should be registered before starting communication.
     * \param   name    The name of the client.
     * \param   service The service address of the client.
     * \param   thread  The thread name where client is running. Required to provide uniqueness.
     * \param   process The name of process. Optional parameter, used to make output in logs.
     * \see     responseRegister
     **/
    void requestRegister( const areg::String & name, const ServiceAddress & service, const areg::String & thread, const areg::String & process ) override;

    /**
     * \brief   Request call.
     *          Sent to unregister connected client.
     * \param   client  The client registration object indicating the unregistered client.
     * \note    Has no response
     **/
    void requestUnregister( const PublicHelloWorld::sClientRegister & client ) override;

    /**
     * \brief   Request call.
     *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
     * \param   clientID    The ID of registered client to make message output
     * \see     responseHelloWorld
     **/
    void requestHelloWorld( uint32_t clientID ) override;

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client  The address of proxy client, which connection status is changed.
     * \param   status  The service consumer connection status.
     * \return  Returns true if connected service consumer is relevant to the provider.
     **/
    bool clientConnected( const ProxyAddress & client, areg::ServiceConnectionState status ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    ClientList      mClientList;    //!< The list of registered clients
    uint32_t    mNumMessages;   //!< The number or processed messages on console.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    PublicHelloWorldService() = delete;
    AREG_NOCOPY_NOMOVE( PublicHelloWorldService );
};

#endif // PUBMESH_COMMON_SRC_PUBLICHELLOWORLDSERVICE_HPP
