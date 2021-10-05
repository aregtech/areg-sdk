/************************************************************************
 * \file        locsvcmesh/src/ClientComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              The component that contains service clients.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locsvcmesh/src/ClientComponent.hpp"

Component * ClientComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ClientComponent(entry, owner);
}

void ClientComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ClientComponent::ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         (owner, entry.mRoleName)

    , mClientMain       ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , mClientSecondary  ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self()) )
    , mClientThird      ( entry.mDependencyServices[2].mRoleName, static_cast<Component &>(self()) )
{

}
