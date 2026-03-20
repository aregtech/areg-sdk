#pragma once

/************************************************************************
 * \file        pubservice/src/TrafficLightService.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a Traffic Light controller, which uses finite-state machine (FSM),
 *              implementation of services and a thread consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/19_pubfsm/services/PowerManagerProviderBase.hpp"
#include "examples/19_pubfsm/services/TrafficControllerProviderBase.hpp"
#include "pubservice/src/TrafficLightActionHandler.hpp"

#include "pubservice/src/PowerControllerClient.hpp"
#include "pubservice/src/TrafficLightFSM.hpp"

/**
 * \brief   The traffic light controller, which is an implementation
 *          on local power controller service and public traffic controller.
 *          The traffic lights are controller by the finite state-machine (FSM),
 *          that triggers the timers to change the states.
 *
 * \note    The demonstrated FSM is not optimized to demonstrate the minimum code.
 *          It is designed to demonstrate the features and the use of FSM with the
 *          help of Areg framework.
 **/
class TrafficLightService final   : public    areg::Component
                            , protected PowerManagerProviderBase
                            , protected TrafficControllerProviderBase
                            , protected TrafficLightActionHandler
{
public:
    /**
     * \brief   Constructor, initializes servicing objects from registry entry.
     **/
    TrafficLightService( const areg::ComponentEntry & entry, areg::ComponentThread & owner );
    /**
     * \brief   Destructor
     **/
    ~TrafficLightService();

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// PowerManagerProviderBase action overrides triggered by state-machine.
/************************************************************************/

    /**
     * \brief   Request call.
     *          Called to power ON the traffic lights.
     * \note    Has no response
     **/
    void request_power_on() final;

    /**
     * \brief   Request call.
     *          Called to power OFF the traffic lights.
     * \note    Has no response
     **/
    void request_power_off() final;

    /**
     * \brief   Request call.
     *          Triggered to start the traffic light
     * \see     request_start_traffic_light
     **/
    void request_start_traffic_light() final;

    /**
     * \brief   Request call.
     *          Call to stop the traffic lights and put them in initialization state.
     * \see     request_stop_traffic_light
     **/
    void request_stop_traffic_light() final;

/************************************************************************/
// TrafficLightActionHandler action overrides
/************************************************************************/

   /**
    * \brief   Action to perform when traffic light is off.
    **/
   void actionPowerOff() final;

   /**
    * \brief   Acton to perform when power on the traffic light.
    **/
   void actionPowerOn() final;

   /**
    * \brief   Action to perform when vehicle light is yellow.
    **/
   void actionVehicleYellow() final;

   /**
    * \brief   Action to perform when vehicle light is red.
    **/
   void actionVehicleRed() final;

   /**
    * \brief   Action to perform when pedestrian light is red.
    **/
   void actionPedestrianRed() final;

   /**
    * \brief   Action to perform when vehicle light is green.
    * \param   isEastWest    Flag, indicating whether this action is triggered for East-West or South-North direction. The default direction is South-North.
    **/
   void actionVehicleGreen( bool isEastWest ) final;

   /**
    * \brief   Action to perform when pedestrian light state is green.
    * \param   isEastWest    Flag, indicating whether this action is triggered for East-West or South-North direction. The default direction is South-North.
    **/
   void actionPedestrianGreen( bool isEastWest ) final;

/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    void startup_component( areg::ComponentThread & comThread ) final;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    void shutdown_component( areg::ComponentThread & comThread ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Wrapper of this pointer.
     **/
    inline TrafficLightService & self();

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    PowerControllerClient   mConsoleInput;  //!< The power controller client object

    TrafficLightFSM         mLightFsm;      //!< The Finite state-machine.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    TrafficLightService() = delete;
    AREG_NOCOPY_NOMOVE( TrafficLightService );
};

//////////////////////////////////////////////////////////////////////////
// inline members
//////////////////////////////////////////////////////////////////////////

inline TrafficLightService & TrafficLightService::self()
{
    return (*this);
}
