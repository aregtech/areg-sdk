/**
 * \file    common/src/ServiceComponent.cpp
 * \brief   Implementation of the service component.
 **/
#include "common/src/ServiceComponent.hpp"

#include <iostream>

Component * ServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceComponent(entry, owner);
}

void ServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & /* entry */)
{
    delete (&compObject);
}

ServiceComponent::ServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( entry, owner )
    , HelloServiceStub  ( static_cast<Component &>(self()) )
{
}

void ServiceComponent::requestHelloService(const String & client)
{
    // output message
    std::cout << "\'Hello Service!\' from " << client << std::endl;

    // reply to unblock the request
    responseHelloService(true);
}
