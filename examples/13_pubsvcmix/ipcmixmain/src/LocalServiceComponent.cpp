/************************************************************************
 * \file        ipcmixmain/src/LocalServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ipcmixmain/src/LocalServiceComponent.hpp"

Component * LocalServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW LocalServiceComponent(entry, owner);
}

void LocalServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

LocalServiceComponent::LocalServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : LocalServicingComponent( entry, owner )
{
}


LocalServiceComponent::~LocalServiceComponent(void)
{
}
