/************************************************************************
 * \file        HelloWorldService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locsvcmesh/src/ServicingComponents.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
// ControllerComponent implementations
//////////////////////////////////////////////////////////////////////////

Component * ControllerComponent::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW ControllerComponent( entry, owner );
}

void ControllerComponent::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

ControllerComponent::ControllerComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component ( entry, owner )
    , mService  ( static_cast<Component &>(self( )), entry.getComponentData( ).alignBool.mElement )
{
}

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent implementations
//////////////////////////////////////////////////////////////////////////

Component * SecondaryComponent::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW SecondaryComponent( entry, owner );
}

void SecondaryComponent::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

SecondaryComponent::SecondaryComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry, owner )
    , mService          ( static_cast<Component &>(self( )), entry.getComponentData( ).alignBool.mElement )
    , mClientMain       ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self( )) )
    , mClientSecondary  ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self( )) )
{
}
