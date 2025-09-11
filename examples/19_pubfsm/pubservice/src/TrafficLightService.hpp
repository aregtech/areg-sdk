#pragma once

/************************************************************************
 * \file        pubservice/src/TrafficLightService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a Traffic Light controller, which uses finite-state machine (FSM),
 *              implementation of services and a thread consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "examples/19_pubfsm/services/PowerManagerStub.hpp"
#include "examples/19_pubfsm/services/TrafficControllerStub.hpp"
#include "pubservice/src/IETrafficLightActionHandler.hpp"

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
 *          help of AREG framework.
 **/
class TrafficLightService   : public    Component
                            , protected PowerManagerStub
                            , protected TrafficControllerStub
                            , protected IETrafficLightActionHandler
{
public:
    /**
     * \brief   Constructor, initializes servicing objects from registry entry.
     **/
    TrafficLightService( const NERegistry::ComponentEntry & entry, ComponentThread & owner );
    /**
     * \brief   Destructor
     **/
    virtual ~TrafficLightService( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// PowerManagerStub action overrides triggered by state-machine.
/************************************************************************/

    /**
     * \brief   Request call.
     *          Called to power ON the traffic lights.
     * \note    Has no response
     **/
    virtual void requestPowerOn( void ) override;

    /**
     * \brief   Request call.
     *          Called to power OFF the traffic lights.
     * \note    Has no response
     **/
    virtual void requestPowerOff( void ) override;

    /**
     * \brief   Request call.
     *          Triggered to start the traffic light
     * \see     responseStartTrafficLight
     **/
    virtual void requestStartTrafficLight( void ) override;

    /**
     * \brief   Request call.
     *          Call to stop the traffic lights and put them in initialization state.
     * \see     responseStopTrafficLight
     **/
    virtual void requestStopTrafficLight( void ) override;

/************************************************************************/
// IETrafficLightActionHandler action overrides
/************************************************************************/

   /**
    * \brief   Action to perform when traffic light is off.
    **/
   virtual void actionPowerOff( void ) override;

   /**
    * \brief   Acton to perform when power on the traffic light.
    **/
   virtual void actionPowerOn( void ) override;

   /**
    * \brief   Action to perform when vehicle light is yellow.
    **/
   virtual void actionVehicleYellow( void ) override;

   /**
    * \brief   Action to perform when vehicle light is red.
    **/
   virtual void actionVehicleRed( void ) override;

   /**
    * \brief   Action to perform when pedestrian light is red.
    **/
   virtual void actionPedestrianRed( void ) override;

   /**
    * \brief   Action to perform when vehicle light is green.
    * \param   isEastWest    Flag, indicating whether this action is triggered for East-West or South-North direction. The default direction is South-North.
    **/
   virtual void actionVehicleGreen( bool isEastWest ) override;

   /**
    * \brief   Action to perform when pedestrian light state is green.
    * \param   isEastWest    Flag, indicating whether this action is triggered for East-West or South-North direction. The default direction is South-North.
    **/
   virtual void actionPedestrianGreen( bool isEastWest ) override;

/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void startupComponent( ComponentThread & comThread ) override;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void shutdownComponent( ComponentThread & comThread ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Wrapper of this pointer.
     **/
    inline TrafficLightService & self( void );

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
    TrafficLightService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TrafficLightService );
};

//////////////////////////////////////////////////////////////////////////
// inline members
//////////////////////////////////////////////////////////////////////////

inline TrafficLightService & TrafficLightService::self( void )
{
    return (*this);
}
