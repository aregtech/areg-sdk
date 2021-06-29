/************************************************************************
 * \file        ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "ServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_10_svchello_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_10_svchello_ServicingComponent_shutdownServiceIntrface);
DEF_TRACE_SCOPE(examples_10_svchello_ServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_10_svchello_ServicingComponent_requestClientShutdown);

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

ServicingComponent::~ServicingComponent(void)
{
}

void ServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_10_svchello_ServicingComponent_startupServiceInterface);

    HelloWorldStub::startupServiceInterface(holder);

    invalidateConnectedClients();
    setRemainOutput(NEHelloWorld::MaxMessages);
}

void ServicingComponent::shutdownServiceIntrface ( Component & holder )
{
    TRACE_SCOPE(examples_10_svchello_ServicingComponent_shutdownServiceIntrface);

    HelloWorldStub::shutdownServiceIntrface(holder);
}

void ServicingComponent::requestHelloWorld(const String & roleName, const String & addMessage /*= "" */)
{
    TRACE_SCOPE(examples_10_svchello_ServicingComponent_requestHelloWorld);

    NEHelloWorld::ConnectionList & list = getConnectedClients();
    LISTPOS pos = list.firstPosition();
    NEHelloWorld::sConnectedClient cl;
    for ( ; pos != NULL; pos = list.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = list.getAt(pos);
        if (roleName == client.ccName)
        {
            TRACE_DBG("Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString(), client.ccID);

            cl.ccName   = client.ccName;
            cl.ccID     = client.ccID;
            break;
        }
    }

    if (pos == NULL)
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

    printf("Client [ %s ] says \"!!!Hello World!!!\". Remain [ %d ].\n", roleName.getString(), outputs - 1);
    if (addMessage.isEmpty() == false)
    {
        printf("\t>>> The additional message: %s.\n", addMessage.getString());
    }
}

void ServicingComponent::requestClientShutdown(unsigned int clientID, const String & roleName)
{
    TRACE_SCOPE(examples_10_svchello_ServicingComponent_requestClientShutdown);

    TRACE_DBG("A client [ %s ] with ID [ %u ] notified shutdown.", roleName.getString(), clientID);
    NEHelloWorld::ConnectionList & list = getConnectedClients();
    LISTPOS pos = list.firstPosition();

    for ( ; pos != NULL; pos = list.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = list.getAt(pos);
        if (client.ccID == clientID)
        {
            ASSERT(client.ccName == roleName);
            list.removeAt(pos);
            notifyConnectedClientsUpdated();
            break;
        }
    }

    ASSERT(pos != NULL);

    if (list.isEmpty() && getRemainOutput() == 0)
    {
        TRACE_INFO("All clients are set message to shutdown, all [ %d ] messages are output, going to shutdown application", NEHelloWorld::MaxMessages);
        Application::signalAppQuit();
    }
}
