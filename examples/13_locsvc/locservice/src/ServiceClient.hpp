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

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/13_locsvc/services/HelloWorldConsumerBase.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//! \brief  A client component to call request, and process response and broadcast.
//!         The requests are triggered on each timer timeout.
class ServiceClient final   : public    areg::Component
                            , protected HelloWorldConsumerBase
                            , private   areg::TimerConsumer
{
private:
    static constexpr uint32_t   TIMEOUT_VALUE   { 100 };    //!< A timeout to trigger request

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

protected:

    /**
     * \brief   The response to hello world request.
     **/
    void response_hello_world() final;

    /**
     * \brief   Server broadcast.
     *          Triggered to notify that  reached the maximum number of requests.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   maxNumber   The maximum number of requests.
     **/
    void broadcast_reached_maximum( int32_t maxNumber ) final;

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
    bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) final;

/************************************************************************/
// TimerConsumer interface finals.
/************************************************************************/
    /**
     * \brief   Triggered when Timer is expired.
     * \param   timer   The timer object that is expired.
     **/
    void process_timer( areg::Timer & timer ) final;

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self()
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
    areg::Timer           mTimer; //!< The timer to trigger to send request to output message

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient() = delete;
    AREG_NOCOPY_NOMOVE( ServiceClient );
};
