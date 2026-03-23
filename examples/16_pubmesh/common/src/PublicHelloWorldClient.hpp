#ifndef PUBMESH_COMMON_SRC_PUBLICHELLOWORLDCLIENT_HPP
#define PUBMESH_COMMON_SRC_PUBLICHELLOWORLDCLIENT_HPP

/************************************************************************
 * \file        common/src/PublicHelloWorldClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "examples/16_pubmesh/services/PublicHelloWorldConsumerBase.hpp"
#include "examples/16_pubmesh/services/SystemShutdownConsumerBase.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
class PublicHelloWorldClient    : private   PublicHelloWorldConsumerBase
                                , private   SystemShutdownConsumerBase
                                , private   areg::TimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Instantiates the component object.
     * \param   dependency  The entry of registry describing the dependency service.
     * \param   owner       The component owning thread.
     * \param   timeout     The timeout in milliseconds to trigger the request to output message
     **/
    PublicHelloWorldClient( const areg::DependencyEntry & dependency, areg::Component & owner, uint32_t timeout );

    /**
     * \brief   Destructor
     **/
    virtual ~PublicHelloWorldClient() = default;

protected:

    /**
     * \brief   Response callback.
     *          Sent when the client is registered. Each registered client has unique ID.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   client  The client registration object that contains unique ID.
     * \see     request_register
     **/
    void response_register( const PublicHelloWorld::sClientRegister & client ) override;

    /**
     * \brief   Response callback.
     *          Called as reply to the request to output message.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientID    Indicates the ID client that made message output
     * \see     request_hello_world
     **/
    void response_hello_world( uint32_t clientID ) override;

    /**
     * \brief   Triggered, when ServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceState description: 
     *          Describes the current state of service.
     * \param   ServiceState    The value of ServiceState attribute.
     * \param   state           The data validation flag.
     **/
    void on_service_state_update( SystemShutdown::RunState ServiceState, areg::DataState state ) override;

/************************************************************************/
// ProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) override;

/************************************************************************/
// TimerConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Triggered when Timer is expired.
     * \param   timer   The timer object that is expired.
     **/
    void process_timer( areg::Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline PublicHelloWorldClient & self()
    {
        return (*this);
    }

    inline areg::String timer_name( areg::Component & owner ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
protected:
    const uint32_t  mMsTimeout; //!< The timeout for timer to trigger message output of remote service
    areg::Timer               mTimer;     //!< The timer to trigger to send request to output message
    PublicHelloWorld::sClientRegister   mClient;    //!< The ID given by service.

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PublicHelloWorldClient() = delete;
    AREG_NOCOPY_NOMOVE( PublicHelloWorldClient );
};

#endif // PUBMESH_COMMON_SRC_PUBLICHELLOWORLDCLIENT_HPP
