/************************************************************************
 * \file        HelloWorldService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locsvcmesh/src/ServicingComponents.hpp"
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
// ControllerComponent implementations
//////////////////////////////////////////////////////////////////////////

ControllerComponent::ControllerComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component ( entry, owner )
    , mService  ( static_cast<areg::Component &>(self( )), entry.data().has_value() && std::any_cast<bool>(entry.data( )) )
{
}

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent implementations
//////////////////////////////////////////////////////////////////////////

SecondaryComponent::SecondaryComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         ( entry, owner )
    , mService          ( static_cast<areg::Component &>(self( )), entry.data().has_value() && std::any_cast<bool>(entry.data()) )
    , mClientMain       ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self( )) )
    , mClientSecondary  ( entry.mDependencyServices[1].mRoleName, static_cast<areg::Component &>(self( )) )
{
}
