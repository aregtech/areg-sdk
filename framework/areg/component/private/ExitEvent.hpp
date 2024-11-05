#ifndef AREG_COMPONENT_PRIVATE_EXITEVENT_HPP
#define AREG_COMPONENT_PRIVATE_EXITEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ExitEvent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Exit Event declaration.
 *              All dispatchers receiving this event should be stopped
 *              and start preparing exit procedure
 *
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Event.hpp"

/**
 * \brief   Special exit event used to indicate completion of a job.
 *          The exit event is a singleton object, which is shared by more than thread.
 *          Normally, used to exit thread.
 **/
class ExitEvent : public Event
{
//////////////////////////////////////////////////////////////////////////
// Declare Event runtime information.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(ExitEvent)

//////////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the instance of special exit event object
     **/
    static ExitEvent & getExitEvent( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Hidden. Instantiated only in static method
     **/
    ExitEvent( void );

    /**
     * \brief   Destructor. Hidden. Cannot be deleted outside of class.
     **/
    virtual ~ExitEvent( void ) = default;

private:
/************************************************************************/
// Event class overrides. Hidden. Cannot be directly called.
/************************************************************************/

    /**
     * \brief   Call to destroy Event object.
     *          Overwrite if there is any special action should be performed
     *          before destroying event object.
     **/
    virtual void destroy( void ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ExitEvent );
};

#endif  // AREG_COMPONENT_PRIVATE_EXITEVENT_HPP
