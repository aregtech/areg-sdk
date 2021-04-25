#ifndef AREG_COMPONENT_CETIMEREVENT_HPP
#define AREG_COMPONENT_CETIMEREVENT_HPP
/************************************************************************
 * \file        areg/src/component/CETimerEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer Event Consumer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/TEEvent.hpp"
#include "areg/src/component/private/CETimerEventData.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class IETimerConsumer;

/************************************************************************
 * Dependencies
 ************************************************************************/
class CETimer;
class CETimerEvent;
class CETimerEventData;
class CEDispatcherThread;

/**
 * \brief       The timer Event is triggered when the system timer is
 *              expired. See details in CETimer. The time processing
 *              function of IETimerConsumer object is triggered in
 *              same thread where the Timer object was requested to be started.
 * 
 * \details     Every time where the Timer is started, the relative expired
 *              time ("up to now") and the event count is sent. When the
 *              Timer is fired, the method ProcessTimer() of IETimerConsumer
 *              instance is triggered in the same thread where the timer
 *              was requested to start and fired timer object is passes as parameter.
 *              The Timer will be fired and the caller will receive processing
 *              request as long, until 'event count' time set in timer
 *              or until the Timer is not manually stopped. The Timer,
 *              which event count is equal to CETimer::CONTINUOUSLY will
 *              run until is not manually stopped. If event count of Timer
 *              is zero, it will not start. For more details see CETimer.
 *
 * \see CETimer
 *
 **/

//////////////////////////////////////////////////////////////////////////
// IETimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   IETimerConsumer class is receiving timer processing
 *          message when timer is expired. Inherit and override
 *          pure virtual function to process Timer.
 **/
class AREG_API IETimerConsumer : public TEEventConsumer<CETimerEventData, const CETimerEventData &>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor
     **/
    IETimerConsumer( void );
    /**
     * \brief   Destructor
     **/
    virtual ~IETimerConsumer( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void ProcessTimer( CETimer & timer ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden overrides
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Automatically triggered when event is dispatched by thread.
     * \param   data    The Timer Event Data object containing Timer object.
     **/
    virtual void ProcessEvent( const CETimerEventData & data);
    /**
     * \brief	Triggered when dispatcher starts to dispatch Timer Event.
     * \param	eventElem   The instance of CETimerEvent. Otherwise, it is ignored.
     **/
    virtual void StartEventProcessing( CEEvent & eventElem);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IETimerConsumer(const IETimerConsumer & /*src*/ );
    const IETimerConsumer& operator = (const IETimerConsumer & /*src*/ );
};

#endif  // AREG_COMPONENT_CETIMEREVENT_HPP
