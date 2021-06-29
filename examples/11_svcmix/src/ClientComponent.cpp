/************************************************************************
 * \file        ClientComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              Main servicing component.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ClientComponent.hpp"

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


ClientComponent::~ClientComponent(void)
{
}
