/************************************************************************
 * \file        SecondaryComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              Main servicing component.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "SecondaryComponent.hpp"

Component * SecondaryComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW SecondaryComponent(entry, owner);
}

SecondaryComponent::SecondaryComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : ServicingComponent(entry, owner)

    , mClientMain       ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , mClientSecondary  ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self()) )
{
}
