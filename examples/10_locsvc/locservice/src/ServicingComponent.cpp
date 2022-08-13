/************************************************************************
 * \file        locservice/src/ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locservice/src/ServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_10_locservice_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestClientShutdown);

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(entry, owner);
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( owner, entry.mRoleName)
    , HelloWorldStub    ( static_cast<Component &>(self()) )
    , mGnerateID        ( 0 )
{

}

void ServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_10_locservice_ServicingComponent_startupServiceInterface);

    HelloWorldStub::startupServiceInterface(holder);

    invalidateConnectedClients();
    setRemainOutput(NEHelloWorld::MaxMessages);
}

void ServicingComponent::requestHelloWorld(const String & roleName)
{
    TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestHelloWorld);

    NEHelloWorld::ConnectionList & list = getConnectedClients();
    NEHelloWorld::ConnectionList::LISTPOS pos = list.firstPosition();
    NEHelloWorld::sConnectedClient cl;
    for ( ; list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = list.valueAtPosition(pos);
        if (roleName == client.ccName)
        {
            TRACE_DBG("Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString(), client.ccID);

            cl.ccName   = client.ccName;
            cl.ccID     = client.ccID;
            break;
        }
    }

    if (list.isInvalidPosition(pos))
    {
        cl.ccID     = ++ mGnerateID;
        cl.ccName   = roleName;

        TRACE_INFO("Registered new client component [ %s ] with ID [ %u ]", cl.ccName.getString(), cl.ccID);

        list.pushLast(cl);

        responseHelloWorld(cl);
        broadcastHelloClients(list);
        notifyConnectedClientsUpdated();
    }
    else
    {
        responseHelloWorld(cl);
    }

    short outputs = getRemainOutput();
    if (outputs == 1)
    {
        setRemainOutput(0);
        broadcastServiceUnavailable();
        TRACE_INFO("Reached maximum to output messages, this should trigger the shutdown procedure.");
    }
    else if (outputs > 1)
    {
        TRACE_DBG("Remain  to output message [ %d ]", outputs - 1);
        setRemainOutput(outputs - 1);
    }
    else
    {
        TRACE_WARN("Still making output of queued requests to print Hello World.");
    }

    printf("\"Hello client [ %s ]!\", remain [ %d ].\n", roleName.getString(), outputs - 1);
}

void ServicingComponent::requestClientShutdown(unsigned int clientID, const String & roleName)
{
    TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestClientShutdown);

    TRACE_DBG("A client [ %s ] with ID [ %u ] notified shutdown.", roleName.getString(), clientID);
    NEHelloWorld::ConnectionList & list = getConnectedClients();
    NEHelloWorld::ConnectionList::LISTPOS pos = list.firstPosition();

    for ( ; list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = list.valueAtPosition(pos);
        if (client.ccID == clientID)
        {
            ASSERT(client.ccName == roleName);
            list.removeAt(pos);
            notifyConnectedClientsUpdated();
            break;
        }
    }

    if (list.isEmpty() && getRemainOutput() == 0)
    {
        TRACE_INFO("All clients are set message to shutdown, all [ %d ] messages are output, going to shutdown application", NEHelloWorld::MaxMessages);
        Application::signalAppQuit();
    }
}
