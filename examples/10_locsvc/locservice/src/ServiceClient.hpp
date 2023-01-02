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
#include "generated/src/HelloWorldClientBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//! \brief  A client component to call request, and process response and broadcast.
//!         The reuqests are triggered on each timer timeout.
class ServiceClient : public    Component
                    , protected HelloWorldClientBase
                    , private   IETimerConsumer
{
private:
    static constexpr unsigned int   TIMEOUT_VALUE   { 100 };    //!< A timeout to trigger request

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

    virtual ~ServiceClient(void) = default;

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
     * \param   maxNumber   The maximum number of reqeusts.
     **/
    virtual void broadcastReachedMaximum( int maxNumber ) override;

/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

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
