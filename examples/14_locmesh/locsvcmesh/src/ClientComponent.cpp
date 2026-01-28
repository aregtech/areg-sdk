/************************************************************************
 * \file        locsvcmesh/src/ClientComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              The component that contains service clients.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locsvcmesh/src/ClientComponent.hpp"

ClientComponent::ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( entry, owner )

    , mClientMain       ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , mClientSecondary  ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self()) )
    , mClientThird      ( entry.mDependencyServices[2].mRoleName, static_cast<Component &>(self()) )
{
}
