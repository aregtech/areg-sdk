/************************************************************************
 * \file        pubservice/src/OptionsServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/OptionsServiceClient.hpp"

Component * OptionsServiceClient::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW OptionsServiceClient( entry, owner );
}

void OptionsServiceClient::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    delete static_cast<OptionsServiceClient *>(&compObject);
}

OptionsServiceClient::OptionsServiceClient( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry.mRoleName )
    , OptionsClientBase ( entry.mRoleName, static_cast<Component &>(self()) )
{
}
