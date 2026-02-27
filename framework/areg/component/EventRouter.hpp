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
class Event;

//////////////////////////////////////////////////////////////////////////
// EventRouter class declarations
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for objects that deliver events to their target threads. Dispatchers are
 *          instances of EventRouter.
 **/
class AREG_API EventRouter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    EventRouter() = default;
    virtual ~EventRouter() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Posts and delivers an event to its target thread or process. Override to implement
     *          routing logic.
     *
     * \param   eventElem       The event object to post.
     * \return  True if the target was found and the event delivered successfully; false otherwise.
     **/
    virtual bool post_event( Event & eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( EventRouter );
};

#endif  // AREG_COMPONENT_EVENTROUTER_HPP
