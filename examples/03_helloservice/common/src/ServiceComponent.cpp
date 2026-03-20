/**
 * \file    common/src/ServiceComponent.cpp
 * \brief   Implementation of the service component.
 **/
#include "common/src/ServiceComponent.hpp"

#include <iostream>

ServiceComponent::ServiceComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component         ( entry, owner )
    , HelloServiceProviderBase  ( static_cast<areg::Component &>(self()) )
{
}

void ServiceComponent::request_hello_service(const areg::String & client)
{
    // output message
    std::cout << "\'Hello Service!\' from " << client << std::endl;

    // reply to unblock the request
    response_hello_service(true);
}
