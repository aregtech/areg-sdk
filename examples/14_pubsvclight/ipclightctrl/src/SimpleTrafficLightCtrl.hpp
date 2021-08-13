#ifndef  AREG_EXAMPLES_14_PUBSVCLIGHT_IPCLIGHTCTRL_SRC_SIMPLETRAFFICLIGHTCTRL_HPP
#define  AREG_EXAMPLES_14_PUBSVCLIGHT_IPCLIGHTCTRL_SRC_SIMPLETRAFFICLIGHTCTRL_HPP
/************************************************************************
 * \file        ipclightctrl/src/SimpleTrafficLightCtrl.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/SimpleTrafficLightStub.hpp"
#include "generated/src/SimpleTrafficSwitchStub.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/TEEvent.hpp"

#include "areg/component/Timer.hpp"

/**
 * \brief   The traffic light switching event data
 **/
class TrafficSwitchData
{
public:
//////////////////////////////////////////////////////////////////////////
// Constructors, destructor.
//////////////////////////////////////////////////////////////////////////
    inline TrafficSwitchData( bool switchOn = true );

    inline TrafficSwitchData( const TrafficSwitchData & src );

    inline ~TrafficSwitchData( void );
    
//////////////////////////////////////////////////////////////////////////
// operators and attributes.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Assigns data from sources.
     **/
    inline const TrafficSwitchData & operator = (const TrafficSwitchData & src );
    
    /**
     * \brief   Returns the data value.
     **/
    inline bool getData( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    bool    mSwitchOn;  //!< ON and OFF state of traffic light.
};

// declaration of custom event.
DECLARE_EVENT(TrafficSwitchData, TrafficSwitchEvent, IETrafficSwitchConsumer);

/**
 * \brief   Traffic light public and local services.
 *          The local service is invisible outside of process and sets ON / OFF state of lights.
 *          The public service broadcasts the light states.
 **/
class SimpleTrafficLightCtrl    : public    Component
                                , public    SimpleTrafficLightStub
                                , public    SimpleTrafficSwitchStub
                                , protected IETimerConsumer
                                , protected IETrafficSwitchConsumer
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
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Private constructor, called from CreateComponent static method.
     * \param   entry   The component entry that contains role name, provided and dependent services.
     * \param   owner   The component owning thread.
     **/
    SimpleTrafficLightCtrl(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   The destructor.
     **/
    virtual ~SimpleTrafficLightCtrl( void );

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitch Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Sets the traffic light ON or OFF.
     * \param   switchOn    If true, the traffic light is switched ON. Otherwise, it is OFF.
     * \note    Has no response
     **/
    virtual void requestSwitchLight( bool switchOn );

    /**
     * \brief  Override operation. Implement this function to receive events and make processing
     * \param  data    The data, which was passed as an event.
     **/
    virtual void processEvent( const TrafficSwitchData & data );

    /**
     * \brief   Automatically triggered when event is dispatched by thread.
     * \param   data    The Timer Event Data object containing Timer object.
     **/
    virtual void processTimer( Timer & timer );

/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder );

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder );

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    inline SimpleTrafficLightCtrl & self( void );

    Timer   mTimer;

    NESimpleTrafficLight::eTrafficLight mPrevState;
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////////
// inline methods
//////////////////////////////////////////////////////////////////////////

inline SimpleTrafficLightCtrl & SimpleTrafficLightCtrl::self( void )
{
    return (*this);
}

inline TrafficSwitchData::TrafficSwitchData( bool switchOn /*= true*/ )
    : mSwitchOn ( switchOn )
{
}

inline TrafficSwitchData::TrafficSwitchData( const TrafficSwitchData & src )
    : mSwitchOn ( src.mSwitchOn )
{
}

inline TrafficSwitchData::~TrafficSwitchData( void )
{
}

inline const TrafficSwitchData & TrafficSwitchData::operator = (const TrafficSwitchData & src )
{
    mSwitchOn   = src.mSwitchOn;
    return (*this);
}

inline bool TrafficSwitchData::getData( void ) const
{
    return mSwitchOn;
}

#endif  // AREG_EXAMPLES_14_PUBSVCLIGHT_IPCLIGHTCTRL_SRC_
