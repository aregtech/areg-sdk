#pragma once

/************************************************************************
 * \file        pubclient/src/TrafficLightClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a Traffic Light controller client to switch lights.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/TrafficControllerClientBase.hpp"

/**
 * \brief   Traffic light controller client, i.e. the lights.
 *          It receives the state of lights to display (output on console).
 **/
class TrafficLightClient    : public    Component
                            , private   TrafficControllerClientBase
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
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Initializes the traffic light controller client.
     * \param   entry   The registry entry to set the role name and the dependencies of components.
     * \param   owner   The instance of component thread.
     **/
    TrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor
     **/
    virtual ~TrafficLightClient(void) = default;

    /**
     * \brief   The wrapper of this pointer.
     **/
    TrafficLightClient & self( void );

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
