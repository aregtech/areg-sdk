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
#include "examples/22_pubwatchdog/services/HelloWatchdogClientBase.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service client, which runs in separate thread.
 *          It sends the command to service to sleep, so that the thread
 *          does not reset watchdog. If watchdog timeout expires, the system
 *          terminates the thread and components and restarts again.
 *          After reaching certain amount of restarts, the application quits.
 **/
class ServiceClient : public    Component
                    , protected HelloWatchdogClientBase
{
private:
    /**
     * \brief   Timeout to wait before sending message
     **/
    static const unsigned int    TIMEOUT_VALUE       /*= 237*/;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

protected:

    /**
     * \brief   Triggered, when ServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ServiceState description:
     *          The state of the service.
     * \param   ServiceState    The value of ServiceState attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onServiceStateUpdate( NEHelloWatchdog::eState ServiceState, NEService::eDataStateType state ) override;

    /**
     * \brief   Response callback.
     *          Triggered to stop the timer.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   timeoutSleep    The timeout in milliseconds while thread was in suspended mode.
     * \see     requestStartSleep
     **/
    virtual void responseStartSleep( unsigned int timeoutSleep ) override;

#if AREG_LOGS
    /**
     * \brief   Overwrite to handle error of StartSleep request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestStartSleepFailed( NEService::eResultType FailureReason ) override;

    /**
     * \brief   Overwrite to handle error of StopService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestStopServiceFailed( NEService::eResultType FailureReason ) override;

    /**
     * \brief   Overwrite to handle error of ShutdownService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestShutdownServiceFailed( NEService::eResultType FailureReason ) override;
#endif  // AREG_LOGS

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

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self( void );

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
    uint32_t mSleepTimeout; //!< The thread sleep timeout
    uint32_t mRestarts;     //!< The number of service restarts.

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceClient );
};

inline ServiceClient & ServiceClient::self( void )
{
    return (*this);
}
