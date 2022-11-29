/************************************************************************
 * \file        pubservice/src/MainServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/MainServiceComponent.hpp"

const unsigned  int MainServiceComponent::LOCAL_TIMEOUT         = 789;

Component * MainServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW MainServiceComponent(entry, owner);
}

void MainServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

MainServiceComponent::MainServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : MainServicingComponent( entry, owner )
    , mLocal                ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()), LOCAL_TIMEOUT )
{
}
