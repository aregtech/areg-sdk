/************************************************************************
 * \file        ipcmixsecond/src/RemoteServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "ipcmixsecond/src/RemoteServiceComponent.hpp"

Component * RemoteServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW RemoteServiceComponent(entry, owner);
}

void RemoteServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

RemoteServiceComponent::RemoteServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : RemoteServicingComponent  ( entry, owner  )

    , mMain                     ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()), RemoteServiceComponent::MAIN_MESSAGE_TIMEOUT  )
    , mRemote                   ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self()), RemoteServiceComponent::REMOTE_MESSAGE_TIMEOUT)
    , mLocal2                   ( entry.mDependencyServices[2].mRoleName, static_cast<Component &>(self()), RemoteServiceComponent::LOCAL_MESSAGE_TIMEOUT )
    , mLocal3                   ( entry.mDependencyServices[3].mRoleName, static_cast<Component &>(self()), RemoteServiceComponent::LOCAL_MESSAGE_TIMEOUT )
{
}

RemoteServiceComponent::~RemoteServiceComponent(void)
{
}
