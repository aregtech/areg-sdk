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
    static const uint32_t    TIMEOUT_VALUE       /*= 237*/;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceClient(const areg::ComponentEntry & entry, ComponentThread & owner);

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
    void onServiceStateUpdate( HelloWatchdog::ComponentState ServiceState, areg::DataState state ) override;

    /**
     * \brief   Response callback.
     *          Triggered to stop the timer.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   timeoutSleep    The timeout in milliseconds while thread was in suspended mode.
     * \see     requestStartSleep
     **/
    void responseStartSleep( uint32_t timeoutSleep ) override;

#if AREG_LOGS
    /**
     * \brief   Overwrite to handle error of StartSleep request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestStartSleepFailed( areg::ResultType FailureReason ) override;

    /**
     * \brief   Overwrite to handle error of StopService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestStopServiceFailed( areg::ResultType FailureReason ) override;

    /**
     * \brief   Overwrite to handle error of ShutdownService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestShutdownServiceFailed( areg::ResultType FailureReason ) override;
#endif  // AREG_LOGS

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
    bool serviceConnected( areg::ServiceConnectionState status, ProxyBase & proxy ) override;

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self();

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
    uint32_t mSleepTimeout; //!< The thread sleep timeout
    uint32_t mRestarts;     //!< The number of service restarts.

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient() = delete;
    AREG_NOCOPY_NOMOVE( ServiceClient );
};

inline ServiceClient & ServiceClient::self()
{
    return (*this);
}
