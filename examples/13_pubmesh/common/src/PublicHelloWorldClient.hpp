#ifndef PUBMESH_COMMON_SRC_PUBLICHELLOWORLDCLIENT_HPP
#define PUBMESH_COMMON_SRC_PUBLICHELLOWORLDCLIENT_HPP

/************************************************************************
 * \file        common/src/PublicHelloWorldClient.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/PublicHelloWorldClientBase.hpp"
#include "generated/src/SystemShutdownClientBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
class PublicHelloWorldClient    : private   PublicHelloWorldClientBase
                                , private   SystemShutdownClientBase
                                , private   IETimerConsumer
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
    PublicHelloWorldClient( const NERegistry::DependencyEntry & dependency, Component & owner, unsigned int timeout );

    /**
     * \brief   Destructor
     **/
    virtual ~PublicHelloWorldClient(void) = default;

protected:

    /**
     * \brief   Response callback.
     *          Sent when the client is registered. Each registered client has unique ID.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   client  The client registration object that contains unique ID.
     * \see     requestRegister
     **/
    virtual void responseRegister( const NEPublicHelloWorld::sClientRegister & client ) override;

    /**
     * \brief   Response callback.
     *          Called as reply to the request to output message.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientID    Indicates the ID client that made message output
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( unsigned int clientID ) override;

    /**
     * \brief   Triggered, when ServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceState description: 
     *          Describes the current state of service.
     * \param   ServiceState    The value of ServiceState attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onServiceStateUpdate( NESystemShutdown::eServiceState ServiceState, NEService::eDataStateType state ) override;

/************************************************************************/
// IEProxyListener Overrides
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
    virtual bool serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy ) override;

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Triggered when Timer is expired.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline PublicHelloWorldClient & self( void )
    {
        return (*this);
    }

    inline String timerName( Component & owner ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
protected:
    const unsigned int  mMsTimeout; //!< The timeout for timer to trigger message output of remote service
    Timer               mTimer;     //!< The timer to trigger to send request to output message
    NEPublicHelloWorld::sClientRegister   mClient;    //!< The ID given by service.

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PublicHelloWorldClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PublicHelloWorldClient );
};

#endif // PUBMESH_COMMON_SRC_PUBLICHELLOWORLDCLIENT_HPP
