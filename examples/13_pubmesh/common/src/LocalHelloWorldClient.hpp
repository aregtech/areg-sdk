#ifndef PUBMESH_COMMON_SRC_LOCALHELLOWORLDCLIENT_HPP
#define PUBMESH_COMMON_SRC_LOCALHELLOWORLDCLIENT_HPP

/************************************************************************
 * \file        common/src/LocalHelloWorldClient.hpp
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
#include "areg/component/IETimerConsumer.hpp"
#include "examples/13_pubmesh/services/LocalHelloWorldClientBase.hpp"
#include "examples/13_pubmesh/services/SystemShutdownClientBase.hpp"

#include "areg/component/Timer.hpp"

//! \brief   A Local service client.
class LocalHelloWorldClient : private   LocalHelloWorldClientBase
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
     * \param   timeout The timeout in milliseconds to trigger the request to output message
     **/
    LocalHelloWorldClient( const NERegistry::DependencyEntry & dependency, Component & owner, unsigned int timeout );

    virtual ~LocalHelloWorldClient(void) = default;

protected:
    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientInfo  The client information set by servicing component. If empty or invalid ID, the message output failed.
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo ) override;

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
    inline LocalHelloWorldClient & self( void )
    {
        return (*this);
    }

    //!< Generated unique timer name.
    inline String timerName( Component & owner ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
    const unsigned int  mMsTimeout; //!< The timeout in milliseconds to set for timer
    Timer               mTimer;     //!< The timer to trigger to send request to output message
    unsigned int        mID;        //!< The ID given by service.

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalHelloWorldClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalHelloWorldClient );
};

#endif // PUBMESH_COMMON_SRC_LOCALHELLOWORLDCLIENT_HPP
