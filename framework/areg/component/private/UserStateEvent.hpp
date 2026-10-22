#ifndef AREG_COMPONENT_PRIVATE_USERSTATEEVENT_HPP
#define AREG_COMPONENT_PRIVATE_USERSTATEEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/UserStateEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the event that announces a user state change to a component thread.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventTemplate.hpp"

namespace areg {

class ComponentThread;

//////////////////////////////////////////////////////////////////////////
// UserStateData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The previous and the new user state.
 **/
class UserStateData
{
public:
    UserStateData( uint32_t previous = 0u, uint32_t current = 0u ) noexcept
        : mPrevious ( previous )
        , mCurrent  ( current )
    {
    }

    UserStateData( const UserStateData & /*src*/ ) = default;
    UserStateData & operator = ( const UserStateData & /*src*/ ) = default;

    uint32_t    mPrevious;  //!< The user state before the change.
    uint32_t    mCurrent;   //!< The new user state.
};

AREG_DECLARE_EVENT(UserStateData, UserStateEvent, UserStateConsumer);

//////////////////////////////////////////////////////////////////////////
// UserStateNotifier class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Receives the user state event on a component thread and calls each component.
 **/
class UserStateNotifier final : public UserStateConsumer
{
public:
    /**
     * \brief   Listens for the user state event on the thread until it is destroyed.
     **/
    explicit UserStateNotifier( ComponentThread & thread );

    ~UserStateNotifier() override;

    /**
     * \brief   Posts the user state change to the component thread.
     **/
    static void announce( ComponentThread & thread, uint32_t previous, uint32_t current );

protected:
    void process_event( const UserStateData & data ) override;

private:
    ComponentThread &   mThread;    //!< The component thread of the components.

private:
    UserStateNotifier() = delete;
    AREG_NOCOPY_NOMOVE( UserStateNotifier );
};

} // namespace areg

#endif  // AREG_COMPONENT_PRIVATE_USERSTATEEVENT_HPP
