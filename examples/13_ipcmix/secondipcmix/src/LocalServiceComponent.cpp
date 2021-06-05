/************************************************************************
 * \file        LocalServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "LocalServiceComponent.hpp"

const unsigned int   LocalServiceComponent::REMOTE_TIMEOUT      = 450;
const unsigned int   LocalServiceComponent::LOCAL_TIMEOUT       = 800;

LocalServiceComponent::LocalServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : LocalServicingComponent   ( entry, owner )
    , mRemote                   ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::REMOTE_TIMEOUT )
    , mLocal                    ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::LOCAL_TIMEOUT  )
{
}


LocalServiceComponent::~LocalServiceComponent(void)
{
}

Component * LocalServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW LocalServiceComponent(entry, owner);
}

void LocalServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}
