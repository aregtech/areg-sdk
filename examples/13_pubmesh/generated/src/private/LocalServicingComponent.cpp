/************************************************************************
 * \file        generated/src/private/LocalServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "generated/src/LocalServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_requestHelloWorld);

LocalServicingComponent::LocalServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName)
    , LocalHelloWorldStub   ( static_cast<Component &>(self()) )
    , mGnerateID            ( 0 )
{
}

void LocalServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_startupServiceInterface);

    LocalHelloWorldStub::startupServiceInterface(holder);

    invalidateConnectedClients();

    TRACE_DBG("Component [ %s ] set maximum allowed output messages [ %d ]", getRoleName().getString(), NELocalHelloWorld::MaxMessages);
    setRemainOutput(NELocalHelloWorld::MaxMessages);
}

void LocalServicingComponent::requestHelloWorld(const String & roleName, const String & addMessage /*= "" */)
{
    TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_requestHelloWorld);

    NELocalHelloWorld::ConnectionList & list = getConnectedClients();
    NELocalHelloWorld::ConnectionList::LISTPOS pos = list.firstPosition();
    NELocalHelloWorld::sConnectedClient cl;
    for ( ; list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        const NELocalHelloWorld::sConnectedClient & client = list.valueAtPosition(pos);
        if (roleName == client.ccName)
        {
            TRACE_DBG("Component [ %s ] found client [ %s ] with ID [ %u ] in the list.", getRoleName().getString(), client.ccName.getString(), client.ccID);

            cl.ccName   = client.ccName;
            cl.ccID     = client.ccID;
            break;
        }
    }

    if (list.isInvalidPosition(pos))
    {
        cl.ccID     = ++ mGnerateID;
        cl.ccName   = roleName;
        
        TRACE_INFO("Component [ %s ] got new client [ %s ] with ID [ %u ]", getRoleName().getString(), cl.ccName.getString(), cl.ccID);

        list.pushLast(cl);

        responseHelloWorld(cl);
        notifyConnectedClientsUpdated();
    }
    else
    {
        responseHelloWorld(cl);
    }

    int outputs = getRemainOutput() - 1;
    setRemainOutput(static_cast<short>(outputs));
    TRACE_DBG("Remain  to output message [ %d ] in local servicing component [ %s ]", outputs, getRoleName().getString() );

    printf(">> LOCAL service [ %s ] client [ %s ]>: \"Hi there!\". Remain [ %d ].\n", roleName.getString(), getRoleName().getString(), outputs);
}
