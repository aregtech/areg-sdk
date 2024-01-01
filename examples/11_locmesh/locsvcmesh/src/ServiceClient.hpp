#pragma once

/************************************************************************
 * \file        ServiceClient.hpp
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
#include "generated/src/HelloWorldClientBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
class ServiceClient : protected HelloWorldClientBase
                    , private   IETimerConsumer
{
    //!< Timeout to wait before sending message
    static constexpr unsigned int   TIMEOUT_VALUE   { 100 };

public:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ServiceClient( const String & roleName, Component & owner );

    virtual ~ServiceClient(void) = default;

protected:

    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientName  The name of connected client that requested to output message.
     * \param   clientId    Thegiven ID of the client that requested to output message.
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( const String & clientName, unsigned int clientId ) override;

    /**
     * \brief   Server broadcast.
     *          Triggered when the service is unavailable.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
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

    /**
     * \brief   Generates unique timer name.
     **/
    inline String timerName( Component & owner ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
    Timer           mTimer; //!< The timer to trigger to send request to output message
    unsigned int    mID;    //!< The ID given by service.

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceClient );
};
