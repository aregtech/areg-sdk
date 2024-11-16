#pragma once

/************************************************************************
 * \file        pubservice/src/PowerControllerClient.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a Traffic Light controller client.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "examples/16_pubfsm/services/PowerManagerClientBase.hpp"
#include "areg/base/IEThreadConsumer.hpp"

#include "areg/base/Thread.hpp"
#include "areg/component/TEEvent.hpp"

#include <string_view>

/**
 * \brief   The power control event data object.
 *          Depending on user input on console, the application generates
 *          the custom event to send to send to power manager client to trigger 
 *          a request. It powers ON and OFF, stops and starts the traffic lights.
 **/
class PowerControllerEventData
{
public:
    /**
     * \brief   The actions to trigger, i.e. the event data.
     **/
    typedef enum class E_Actions
    {
          NoAction          //!< No action
        , ActionPowerOff    //!< Power OFF the traffic lights
        , ActionPowerOn     //!< Power ON the traffic lights
        , ActionStartLight  //!< Start the traffic lights
        , ActionStopLight   //!< Stop the traffic lights.
    } eAction;

    /**
     * \brief   Converts the value of PowerControllerEventData::eAction into the string.
     *          Used to make logging.
     **/
    inline static const char * getString( PowerControllerEventData::eAction action );

//////////////////////////////////////////////////////////////////////////
// Constructors, destructor, operators, attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    inline PowerControllerEventData( void );
    /**
     * \brief   Set the initial value of event data.
     **/
    inline PowerControllerEventData( PowerControllerEventData::eAction action );
    /**
     * \brief   Copy the data from given source
     **/
    inline PowerControllerEventData( const PowerControllerEventData & src );
    /**
     * \brief   Move the data from given source
     **/
    inline PowerControllerEventData( PowerControllerEventData && src );
    /**
     * \brief   Copy the data from given source
     **/
    inline void operator = ( const PowerControllerEventData & src );
    /**
     * \brief   Move the data from given source
     **/
    inline void operator = ( PowerControllerEventData && src );
    /**
     * \brief   Returns the event data value.
     **/
    inline PowerControllerEventData::eAction getAction( void ) const;

private:
    PowerControllerEventData::eAction     mAction;  //!< The power controller event data value.
};

//////////////////////////////////////////////////////////////////////////
// Declare the Power Controller Event and the event consumer objects.
//////////////////////////////////////////////////////////////////////////
DECLARE_EVENT(PowerControllerEventData, PowerControllerEvent, IEPowerControllerEventConsumer);

/**
 * \brief   The actual power controller service client.
 *          The client contains a thread that gets user input from console,
 *          which generates the custom event and sends to the client.
 *          Depending on value, the client calls appropriate request methods to
 *          execute on local power controller service side.
 *          As soon as the power controller gets request to start, the traffic
 *          lights start automatically to run.
 **/
class PowerControllerClient : public    PowerManagerClientBase
                            , protected IEThreadConsumer
                            , protected IEPowerControllerEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of a thread that gets user inputs
     **/
    static constexpr std::string_view ConsoleThreadName { "ConsoleThread" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the client object.
     * \param   roleName    The role name of the power controller service.
     * \param   owner       The client owning component.
     **/
    PowerControllerClient( const char* roleName, Component & owner );
    /**
     * \brief   Destructor.
     **/
    virtual ~PowerControllerClient( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************
 * Response StartTrafficLight
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Sent as a response to start the traffic light. The traffic light can be started when it is powered ON and in initialization state.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   Success Flag, indicating whether the operation succeeded or not.
     *          This flag is 'true' if lights are initialization state of if traffic light is already functioning.
     *          This flag is 'false' if lights are OFF.
     * \see     requestStartTrafficLight
     **/
    virtual void responseStartTrafficLight( bool Success ) override;

/************************************************************************
 * Response StopTrafficLight
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response to stop the traffic lights.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   Success Flag, indicating whether the request was processed with success or not.
     *          This flag is 'true' if traffic light are functioning or lights are in initialization state.
     *          This flag is 'false' if traffic lights are OFF.
     * \see     requestStopTrafficLight
     **/
    virtual void responseStopTrafficLight( bool Success ) override;

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   This callback function is called from Thread object, when it is 
     *          running and fully operable. If thread needs run in loop, the loop 
     *          should be implemented here. When consumer exits this function, 
     *          the thread will complete work. To restart thread running, 
     *          createThread() method should be called again.
     **/
    virtual void onThreadRuns( void ) override;

/************************************************************************/
// IEPowerControllerEventConsumer overrides
/************************************************************************/

    /**
     * \brief  Override operation. Implement this function to receive events and make processing
     * \param  data    The data, which was passed as an event.
     **/
    virtual void processEvent( const PowerControllerEventData & data ) override;

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
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    inline PowerControllerClient & self( void );    //!< The wrapper of this pointer

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The thread object to get user inputs from console.
     **/
    Thread  mConsole;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    PowerControllerClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PowerControllerClient );
};

//////////////////////////////////////////////////////////////////////////
// inline members
//////////////////////////////////////////////////////////////////////////

inline PowerControllerClient & PowerControllerClient::self( void )
{
    return (*this);
}

inline PowerControllerEventData::PowerControllerEventData(void)
    : mAction   (PowerControllerEventData::eAction::NoAction)
{
}

inline PowerControllerEventData::PowerControllerEventData(PowerControllerEventData::eAction action)
    : mAction   ( action )
{
}

inline PowerControllerEventData::PowerControllerEventData(const PowerControllerEventData & src)
    : mAction   ( src.mAction )
{
}

inline void PowerControllerEventData::operator = (const PowerControllerEventData & src)
{
    mAction = src.mAction;
}

inline PowerControllerEventData::eAction PowerControllerEventData::getAction(void) const
{
    return mAction;
}

const char * PowerControllerEventData::getString(PowerControllerEventData::eAction action)
{
    switch (action)
    {
    case PowerControllerEventData::eAction::NoAction:
        return "PowerControllerEventData::NoAction";
    case PowerControllerEventData::eAction::ActionPowerOff:
        return "PowerControllerEventData::ActionPowerOff";
    case PowerControllerEventData::eAction::ActionPowerOn:
        return "PowerControllerEventData::ActionPowerOn";
    case PowerControllerEventData::eAction::ActionStartLight:
        return "PowerControllerEventData::ActionStartLight";
    case PowerControllerEventData::eAction::ActionStopLight:
        return "PowerControllerEventData::ActionStopLight";
    default:
        return "ERR: Undefined PowerControllerEventData::eAction value!";
    }
}
