#pragma once

/************************************************************************
 * \file        locservice/src/ServiceClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloWatchdogClientBase.hpp"

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
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor
     **/
    virtual ~ServiceClient(void) = default;

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

/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

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
