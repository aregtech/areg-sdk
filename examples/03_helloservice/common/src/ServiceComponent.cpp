/**
 * \file    common/src/ServiceComponent.cpp
 * \brief   Implementation of the service component.
 **/
#include "common/src/ServiceComponent.hpp"

#include <iostream>

ServiceComponent::ServiceComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component         ( entry, owner )
    , HelloServiceStub  ( static_cast<areg::Component &>(self()) )
{
}

void ServiceComponent::requestHelloService(const areg::String & client)
{
    // output message
    std::cout << "\'Hello Service!\' from " << client << std::endl;

    // reply to unblock the request
    responseHelloService(true);
}
