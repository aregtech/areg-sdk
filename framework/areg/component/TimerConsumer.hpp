#ifndef AREG_COMPONENT_TIMERCONSUMER_HPP
#define AREG_COMPONENT_TIMERCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/TimerConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer Event Consumer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/private/TimerEventData.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class TimerConsumer;

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class Timer; }
class TimerEventData;
namespace areg { class DispatcherThread; }

namespace areg
{
    /**
     * \brief   The timer Event is triggered when a Timer is expired and it should
     *          be processed in dispatcher thread.
     * 
     *          For details of setting up and starting the timer, see Timer class. 
     * \see Timer
     **/

    //////////////////////////////////////////////////////////////////////////
    // TimerConsumer class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   TimerConsumer class is receiving timer processing
     *          message when timer is expired. Inherit and override
     *          pure virtual function to process Timer.
     **/
    class AREG_API TimerConsumer : public  TimerEventConsumerBase
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor. Protected
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Constructor
         **/
        TimerConsumer() = default;
        /**
         * \brief   Destructor
         **/
        virtual ~TimerConsumer() = default;

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    protected:
    /************************************************************************/
    // TimerConsumer interface overrides.
    /************************************************************************/

        /**
         * \brief   Triggered when Timer is expired. 
         *          The passed Timer parameter is indicating object, which has been expired.
         *          Overwrite method to receive messages.
         * \param   timer   The timer object that is expired.
         **/
        virtual void processTimer( areg::Timer & timer ) = 0;

        /**
         * \brief   Automatically triggered when event is dispatched by thread.
         * \param   data    The Timer Event Data object containing Timer object.
         **/
        void processEvent( const TimerEventData & data) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden overrides
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief	Triggered when dispatcher starts to dispatch Timer Event.
         * \param	eventElem   The instance of TimerEvent. Otherwise, it is ignored.
         **/
        void startEventProcessing( areg::Event & eventElem) override;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( TimerConsumer );
    };

} // namespace areg
#endif  // AREG_COMPONENT_TIMERCONSUMER_HPP
