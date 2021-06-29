/************************************************************************
 * \file        MainComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              Main servicing component.
 ************************************************************************/

#include "MainComponent.hpp"

Component * MainComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW MainComponent(entry, owner);
}

MainComponent::MainComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : ServicingComponent(entry, owner)
{

}


MainComponent::~MainComponent(void)
{
}
