#pragma once

/************************************************************************
 * \file        pubservice/src/PublicServiceComponent.hpp
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
#include "examples/16_pubmesh/services/SystemShutdownStub.hpp"
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
    static constexpr uint32_t  LOCAL_TIMEOUT   { 555 };//!< Timeout to output message in local service.

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    PublicServiceComponent( const areg::ComponentEntry & entry, ComponentThread & owner );

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
    void startupComponent( ComponentThread & comThread ) override;

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client  The address of proxy client, which connection status is changed.
     * \param   status  The service consumer connection status.
     * \return  Returns true if connected service consumer is relevant to the provider.
     **/
    bool clientConnected(const ProxyAddress & client, areg::ServiceConnectionState status) override;

    /**
     * \brief   Request call.
     *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
     * \param   clientID    The ID of registered client to make message output
     * \see     responseHelloWorld
     **/
    void requestHelloWorld( uint32_t clientID ) override;

    /**
     * \brief   Request call.
     *          The request to shutdown the system.
     * \note    Has no response
     **/
    void requestSystemShutdown() override;

private:
//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    LocalHelloWorldClient   mLocalClient;   //! Client of local service

    inline PublicServiceComponent & self()
    {
        return (*this);
    }


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    PublicServiceComponent() = delete;
    AREG_NOCOPY_NOMOVE( PublicServiceComponent );
};
