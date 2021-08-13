/************************************************************************
 * \file        ipcmixclient/src/LocalServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ipcmixclient/src/LocalServiceComponent.hpp"

const unsigned int  LocalServiceComponent::MAIN_TIMEOUT     = 1000;
const unsigned int  LocalServiceComponent::SECOND_TIMEOUT   = 2000;
const unsigned int  LocalServiceComponent::THIRD_TIMEOUT    = 3000;
const unsigned int  LocalServiceComponent::LOCAL_TIMEOUT    = 4000;

LocalServiceComponent::LocalServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : LocalServicingComponent   ( entry, owner )
    , mMain                     ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::MAIN_TIMEOUT   )
    , mSecond                   ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::SECOND_TIMEOUT )
    , mThird                    ( entry.mDependencyServices[2].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::THIRD_TIMEOUT  )
    , mLocal                    ( entry.mDependencyServices[3].mRoleName, static_cast<Component &>(self()), LocalServiceComponent::LOCAL_TIMEOUT  )
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
