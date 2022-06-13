/************************************************************************
 * \file        ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
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


DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServicingComponent_requestClientShutdown);

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( owner, entry.mRoleName)
    , HelloWorldStub    ( static_cast<Component &>(self()) )
    , mGnerateID        ( 0 )
    , mIsMain           ( entry.getComponentData().alignBool.mElement )
{
}

void ServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServicingComponent_startupServiceInterface);

    HelloWorldStub::startupServiceInterface(holder);

    invalidateConnectedClients();

    TRACE_DBG("Component [ %s ] set maximum allowed output messages [ %d ]", getRoleName().getString(), NEHelloWorld::MaxMessages);
    setRemainOutput(NEHelloWorld::MaxMessages);
}

void ServicingComponent::requestHelloWorld(const String & roleName, const String & addMessage /*= "" */)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServicingComponent_requestHelloWorld);

    NEHelloWorld::ConnectionList & list = getConnectedClients();
    NEHelloWorld::ConnectionList::LISTPOS pos = list.firstPosition();
    NEHelloWorld::sConnectedClient cl;
    for ( ; list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = list.valueAtPosition(pos);
        if (roleName == client.ccName)
        {
            TRACE_DBG("Component [ %s ] found client [ %s ] with ID [ %u ] in the list.", getRoleName().getString(), client.ccName.getString(), client.ccID);

            cl.ccName   = client.ccName;
            cl.ccID     = client.ccID;
            break;
        }
    }

    if (list.isEndPosition(pos))
    {
        cl.ccID     = ++ mGnerateID;
        cl.ccName   = roleName;
        
        TRACE_INFO("Component [ %s ] got new client [ %s ] with ID [ %u ]", getRoleName().getString(), cl.ccName.getString(), cl.ccID);

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
        TRACE_INFO("[ %s ] component [ %s ] reached maximum messages.", mIsMain ? "MAIN" : "SECONDARY", getRoleName().getString());
        setRemainOutput(0);

        if (mIsMain)
        {
            TRACE_INFO("Main component triggers shutdown procedure");
            broadcastServiceUnavailable();
        }
    }
    else if (outputs > 1)
    {
        TRACE_DBG("Remain  to output message [ %d ]", outputs - 1);
        setRemainOutput(outputs - 1);
    }
    else
    {
        TRACE_WARN("<< Component [ %s ] reached maximum message numbers, but still makes output. >>", getRoleName().getString());
    }

    printf("Client [ %s ] says \"!!!Hello World!!!\". Remain [ %d ].\n", roleName.getString(), outputs - 1);
    if (addMessage.isEmpty() == false)
    {
        printf("\t>>> The additional message: %s.\n", addMessage.getString());
    }
}

void ServicingComponent::requestClientShutdown(unsigned int clientID, const String & roleName)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServicingComponent_requestClientShutdown);

    TRACE_DBG("A client [ %s ] with ID [ %u ] notified shutdown.", roleName.getString(), clientID);
    NEHelloWorld::ConnectionList & list = getConnectedClients();
    NEHelloWorld::ConnectionList::LISTPOS pos = list.firstPosition();

    for ( ; list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = list.valueAtPosition(pos);
        if (client.ccID == clientID)
        {
            TRACE_DBG("Client [ %s ] with ID [ %d ] in component [ %s ] is removed", roleName.getString(), clientID, getRoleName().getString());
            ASSERT(client.ccName == roleName);
            list.removeAt(pos);
            notifyConnectedClientsUpdated();
            break;
        }
    }

    if (mIsMain && list.isEmpty() && getRemainOutput() == 0)
    {
        TRACE_INFO("All clients are set message to shutdown, all [ %d ] messages are output, going to shutdown application", NEHelloWorld::MaxMessages);
        Application::signalAppQuit();
    }
}
