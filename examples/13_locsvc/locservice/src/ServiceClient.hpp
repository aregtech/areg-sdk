#pragma once

/************************************************************************
 * \file        locservice/src/ServiceClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "examples/13_locsvc/services/HelloWorldClientBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//! \brief  A client component to call request, and process response and broadcast.
//!         The requests are triggered on each timer timeout.
class ServiceClient : public    Component
                    , protected HelloWorldClientBase
                    , private   IETimerConsumer
{
private:
    static constexpr unsigned int   TIMEOUT_VALUE   { 100 };    //!< A timeout to trigger request

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

protected:

    /**
     * \brief   The response to hello world request.
     **/
    virtual void responseHelloWorld( void ) override;

    /**
     * \brief   Server broadcast.
     *          Triggered to notify that  reached the maximum number of requests.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   maxNumber   The maximum number of requests.
     **/
    virtual void broadcastReachedMaximum( int maxNumber ) override;

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
    inline ServiceClient & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
    Timer           mTimer; //!< The timer to trigger to send request to output message

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceClient );
};
