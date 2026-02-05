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
#include "areg/logging/GELog.h"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
// ControllerComponent implementations
//////////////////////////////////////////////////////////////////////////

ControllerComponent::ControllerComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component ( entry, owner )
    , mService  ( static_cast<Component &>(self( )), entry.getData().has_value() && std::any_cast<bool>(entry.getData( )) )
{
}

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent implementations
//////////////////////////////////////////////////////////////////////////

SecondaryComponent::SecondaryComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry, owner )
    , mService          ( static_cast<Component &>(self( )), entry.getData().has_value() && std::any_cast<bool>(entry.getData()) )
    , mClientMain       ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self( )) )
    , mClientSecondary  ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self( )) )
{
}
