#ifndef AREG_COMPONENT_IETIMERCONSUMER_HPP
#define AREG_COMPONENT_IETIMERCONSUMER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/IETimerConsumer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Timer Event Consumer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/component/private/TimerEventData.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class IETimerConsumer;

/************************************************************************
 * Dependencies
 ************************************************************************/
class Timer;
class TimerEventData;
class DispatcherThread;

/**
 * \brief   The timer Event is triggered when a Timer is expired and it should
 *          be processed in dispatcher thread.
 * 
 *          For details of setting up and starting the timer, see Timer class. 
 * \see Timer
 **/

//////////////////////////////////////////////////////////////////////////
// IETimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   IETimerConsumer class is receiving timer processing
 *          message when timer is expired. Inherit and override
 *          pure virtual function to process Timer.
 **/
class AREG_API IETimerConsumer : public  IETimerEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor
     **/
    IETimerConsumer( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IETimerConsumer( void ) = default;

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
    virtual void processTimer( Timer & timer ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden overrides
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Automatically triggered when event is dispatched by thread.
     * \param   data    The Timer Event Data object containing Timer object.
     **/
    virtual void processEvent( const TimerEventData & data) override;

    /**
     * \brief	Triggered when dispatcher starts to dispatch Timer Event.
     * \param	eventElem   The instance of TimerEvent. Otherwise, it is ignored.
     **/
    virtual void startEventProcessing( Event & eventElem) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IETimerConsumer );
};

#endif  // AREG_COMPONENT_IETIMERCONSUMER_HPP
