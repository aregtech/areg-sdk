/************************************************************************
 * \file        pubsvcmesh/src/LocalServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubsvcmesh/src/LocalServiceComponent.hpp"

Component * LocalServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW LocalServiceComponent(entry, owner);
}

void LocalServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

LocalServiceComponent::LocalServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : LocalServicingComponent   ( entry, owner )
    , mRemote                   ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::REMOTE_TIMEOUT )
    , mLocal                    ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::LOCAL_TIMEOUT  )
{
}
