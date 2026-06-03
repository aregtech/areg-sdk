#ifndef AREG_COMPONENT_EXITEVENT_HPP
#define AREG_COMPONENT_EXITEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ExitEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Exit Event declaration.
 *              All dispatchers receiving this event should be stopped
 *              and start preparing exit procedure
 *
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Event.hpp"
namespace areg {

/**
 * \brief   Singleton event used to signal thread completion. Shared across multiple threads.
 **/
class AREG_API ExitEvent final : public Event
{

    AREG_DECLARE_RUNTIME_EVENT(ExitEvent)

//////////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the singleton exit event instance.
     **/
    [[nodiscard]]
    static const ExitEvent & exit_event() noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ExitEvent();

    ~ExitEvent() override = default;

private:
/************************************************************************/
// Event class overrides. Hidden. Cannot be directly called.
/************************************************************************/

    /**
     * \brief   Destroys the event object. Override to perform cleanup before destruction.
     **/
    void destroy_event() noexcept final;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ExitEvent );
};

} // namespace areg
#endif  // AREG_COMPONENT_EXITEVENT_HPP
