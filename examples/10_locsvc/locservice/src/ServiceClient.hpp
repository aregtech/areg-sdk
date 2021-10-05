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

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
class ServiceClient : public    Component
                    , protected HelloWorldClientBase
                    , private   IETimerConsumer
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
     * \brief   Triggered, when ConnectedClients attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ConnectedClients description: 
     *          The list of connected clients. Updated each time when new client requests to output Hello World message.
     * \param   ConnectedClients    The value of ConnectedClients attribute.
     * \param   state               The data validation flag.
     **/
    virtual void onConnectedClientsUpdate( const NEHelloWorld::ConnectionList & ConnectedClients, NEService::eDataStateType state ) override;

        /**
     * \brief   Triggered, when RemainOutput attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes RemainOutput description: 
     *          Remaining number of outputs to print Hello World.
     * \param   RemainOutput    The value of RemainOutput attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onRemainOutputUpdate( short RemainOutput, NEService::eDataStateType state ) override;

    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientInfo  The client information set by servicing component. If empty or invalid ID, the message output failed.
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo ) override;

    /**
     * \brief   Server broadcast.
     *          Broadcast to notify all clients about connection
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientList  DESCRIPTION MISSED
     **/
    virtual void broadcastHelloClients( const NEHelloWorld::ConnectionList & clientList ) override;

    /**
     * \brief   Server broadcast.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     **/
    virtual void broadcastServiceUnavailable( void ) override;

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

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self( void );

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

inline ServiceClient & ServiceClient::self( void )
{
    return (*this);
}
