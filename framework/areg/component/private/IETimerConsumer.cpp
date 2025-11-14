/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/IETimerConsumer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Timer Event Consumer.
 *
 ************************************************************************/
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"

//////////////////////////////////////////////////////////////////////////
// IETimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////

void IETimerConsumer::processEvent( const TimerEventData & /* data */ )
{
    ASSERT(false);
}

void IETimerConsumer::startEventProcessing( Event& eventElem )
{
    TimerEvent* timerEvent = static_cast<TimerEvent *>( RUNTIME_CAST(&eventElem, TimerEvent) );
    Timer *timer = timerEvent != nullptr ? timerEvent->getData().getTimer() : nullptr;
    if (timer != nullptr )
    {
        processTimer(*timer);
    }
}
