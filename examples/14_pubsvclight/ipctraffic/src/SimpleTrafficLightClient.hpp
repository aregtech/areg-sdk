#ifndef  AREG_EXAMPLES_14_PUBSVCLIGHT_IPCTRAFFIC_SRC_SIMPLETRAFFICLIGHTCLIENT_HPP
#define  AREG_EXAMPLES_14_PUBSVCLIGHT_IPCTRAFFIC_SRC_SIMPLETRAFFICLIGHTCLIENT_HPP
/************************************************************************
 * \file        ipctraffic/src/SimpleTrafficLightClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/SimpleTrafficLightClientBase.hpp"

#include "generated/src/NECommon.hpp"

/**
 * \brief   The simple traffic light service client.
 **/
class SimpleTrafficLightClient  : public    Component
                                , protected SimpleTrafficLightClientBase
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants.
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
// Statics and constants.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Private constructor, called from CreateComponent static method.
     * \param   entry   The component entry that contains information of role name and dependent services.
     * \param   owner   The component owning thread.
     **/
    SimpleTrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Hidden destructor.
     **/
    virtual ~SimpleTrafficLightClient( void );

//////////////////////////////////////////////////////////////////////////
// Protected members.
//////////////////////////////////////////////////////////////////////////
protected:

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
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

    /**
     * \brief   Triggered, when SouthNorth attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes SouthNorth description: 
     *          The light state of south-north traffic.
     * \param   SouthNorth  The value of SouthNorth attribute.
     * \param   state       The data validation flag.
     **/
    virtual void onSouthNorthUpdate( NESimpleTrafficLight::eTrafficLight SouthNorth, NEService::eDataStateType state );

    /**
     * \brief   Triggered, when EastWest attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes EastWest description: 
     *          The light state of east-west traffic.
     * \param   EastWest    The value of EastWest attribute.
     * \param   state       The data validation flag.
     **/
    virtual void onEastWestUpdate( NESimpleTrafficLight::eTrafficLight EastWest, NEService::eDataStateType state );

    /**
     * \brief   Server broadcast.
     *          Triggered when the light state of traffic is changed.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   SouthNorth  The light state of south-north traffic.
     * \param   EastWest    The light state of east-west traffic.
     **/
    virtual void broadcastLightChanged( NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest );

//////////////////////////////////////////////////////////////////////////
// Hidden members.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns instance of SimpleTrafficLightClient.
     **/
    inline SimpleTrafficLightClient & self(void);

    /**
     * \brief   Makes message outputs on console.
     **/
    inline void outputState(NESimpleTrafficLight::eTrafficLight lightState);

private:
    /**
     * \brief   The symbolic traffic direction that is South-North or East-West.
     **/
    NECommon::eTrafficDirection  mTrafficDirection;
};

//////////////////////////////////////////////////////////////////////////
// Inline methods
//////////////////////////////////////////////////////////////////////////

inline SimpleTrafficLightClient & SimpleTrafficLightClient::self(void)
{
    return (*this);
}

#endif  // AREG_EXAMPLES_14_PUBSVCLIGHT_IPCTRAFFIC_SRC_SIMPLETRAFFICLIGHTCLIENT_HPP
