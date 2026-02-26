#ifndef AREG_COMPONENT_EVENTROUTER_HPP
#define AREG_COMPONENT_EVENTROUTER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/EventRouter.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Router interface
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class Event; }

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // EventRouter class declarations
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Event Route interface. Defines class, which is delivering
     *          Event to its target thread.
     * 
     *          Dispatcher classes are instances of EventRouter and
     *          have implemented post method to find target thread and
     *          delivery event for further processing.
     *
     **/
    class AREG_API EventRouter
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor. Protected
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Protected constructor and destructor.
         **/
        EventRouter() = default;
        virtual ~EventRouter() = default;

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief	Posts event and delivers to its target thread / process.
         * \param	eventElem	Event object to post.
         * \return	Returns true if target was found and the event
         *          delivered with success. Otherwise it returns false.
         **/
        virtual bool postEvent( areg::Event & eventElem ) = 0;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( EventRouter );
    };

} // namespace areg
#endif  // AREG_COMPONENT_EVENTROUTER_HPP
