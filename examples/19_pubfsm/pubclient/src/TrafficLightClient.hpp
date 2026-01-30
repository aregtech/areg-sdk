#pragma once

/************************************************************************
 * \file        pubclient/src/TrafficLightClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a Traffic Light controller client to switch lights.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "examples/19_pubfsm/services/TrafficControllerClientBase.hpp"

/**
 * \brief   Traffic light controller client, i.e. the lights.
 *          It receives the state of lights to display (output on console).
 **/
class TrafficLightClient    : public    Component
                            , private   TrafficControllerClientBase
{
public:
    TrafficLightClient(const NERegistry::ComponentEntry& entry, ComponentThread& owner);

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Triggered, when TrafficSouthNorth attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes TrafficSouthNorth description: 
     *          The south-north traffic.
     * \param   TrafficSouthNorth   The value of TrafficSouthNorth attribute.
     * \param   state               The data validation flag.
     **/
    virtual void onTrafficSouthNorthUpdate( const NETrafficController::sTrafficLight & TrafficSouthNorth, NEService::eDataStateType state ) override;

    /**
     * \brief   Triggered, when TrafficEastWest attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes TrafficEastWest description: 
     *          The east-west traffic
     * \param   TrafficEastWest The value of TrafficEastWest attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onTrafficEastWestUpdate( const NETrafficController::sTrafficLight & TrafficEastWest, NEService::eDataStateType state ) override;

    /**
     * \brief   Server broadcast.
     *          Broadcasts the light states of south-north direction.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   LightVehicle    Light state for vehicles
     * \param   LightPedestrian Light state for pedestrians.
     **/
    virtual void broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian ) override;

    /**
     * \brief   Server broadcast.
     *          Broadcasts the light states of east-west direction.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   LightVehicle    Light state for vehicles
     * \param   LightPedestrian Light state for pedestrians.
     **/
    virtual void broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian ) override;

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
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The flag, indicating whether it is a symbolic south-north or east-west light controller client.
     **/
    bool    mIsEastWest;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TrafficLightClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TrafficLightClient );
};
