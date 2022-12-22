#pragma once

/************************************************************************
 * \file        pubclient/src/TrafficLightClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
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

//! The simple service client, which receives data update notifications.
class TrafficLightClient    : public    Component
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
    TrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Hidden destructor.
     **/
    virtual ~TrafficLightClient( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Protected members.
//////////////////////////////////////////////////////////////////////////
protected:

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

    /**
     * \brief   Triggered, when SouthNorth attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes SouthNorth description: 
     *          The light state of south-north traffic.
     * \param   SouthNorth  The value of SouthNorth attribute.
     * \param   state       The data validation flag.
     **/
    virtual void onSouthNorthUpdate( NESimpleTrafficLight::eTrafficLight SouthNorth, NEService::eDataStateType state ) override;

    /**
     * \brief   Triggered, when EastWest attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes EastWest description: 
     *          The light state of east-west traffic.
     * \param   EastWest    The value of EastWest attribute.
     * \param   state       The data validation flag.
     **/
    virtual void onEastWestUpdate( NESimpleTrafficLight::eTrafficLight EastWest, NEService::eDataStateType state ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden members.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns instance of TrafficLightClient.
     **/
    inline TrafficLightClient & self( void )
    {
        return (*this);
    }

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
