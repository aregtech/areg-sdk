/************************************************************************
 * \file        shareipcmix/src/private/RemoteServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "shareipcmix/src/RemoteServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_shutdownServiceIntrface);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestClientShutdown);

RemoteServicingComponent::RemoteServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( owner, entry.mRoleName                        )
    , RemoteHelloWorldStub    ( static_cast<Component &>(self())              )
    , mGnerateID        ( 0                                             )
    , mIsMain           ( entry.getComponentData().alignBool.mElement   )
{

}

RemoteServicingComponent::~RemoteServicingComponent(void)
{
}

void RemoteServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_startupServiceInterface);

    RemoteHelloWorldStub::startupServiceInterface(holder);

    invalidateConnectedClients();

    TRACE_DBG("Component [ %s ] set maximum allowed output messages [ %d ]", getRoleName().getString(), NERemoteHelloWorld::MaxMessages);
    setRemainOutput(NERemoteHelloWorld::MaxMessages);
}

void RemoteServicingComponent::shutdownServiceIntrface ( Component & holder )
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_shutdownServiceIntrface);
    TRACE_WARN("Component [ %s ] shuts down service.", getRoleName().getString());

    RemoteHelloWorldStub::shutdownServiceIntrface(holder);
}

void RemoteServicingComponent::requestHelloWorld(const String & roleName, const String & addMessage /*= "" */)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestHelloWorld);

    NERemoteHelloWorld::ConnectionList & list = getConnectedClients();
    LISTPOS pos = list.firstPosition();
    NERemoteHelloWorld::sConnectedClient cl;

    for ( ; pos != NULL; pos = list.nextPosition(pos))
    {
        const NERemoteHelloWorld::sConnectedClient & client = list.getAt(pos);
        if (roleName == client.ccName)
        {
            TRACE_DBG("Component [ %s ] found client [ %s ] with ID [ %u ] in the list.", getRoleName().getString(), client.ccName.getString(), client.ccID);

            cl.ccName   = client.ccName;
            cl.ccID     = client.ccID;
            break;
        }
    }

    if (pos == NULL)
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
        outputs -= 1;
        TRACE_INFO(">> Remote %s component [ %s ] reached maximum messages.", mIsMain ? "main" : "", getRoleName().getString());
    }
    else if (outputs > 1)
    {
        outputs -= 1;
        TRACE_DBG("Remain  to output message [ %d ]", outputs);
    }
    else if (mIsMain == false)
    {
        outputs -= 1;
        TRACE_DBG("Remote component [ %s ] continues to make output.", getRoleName().getString());
    }
    else
    {
        TRACE_INFO("Remote main component ignores to reduce available output value.");
    }

    printf(">>> REMOTE %s client [ %s ] says \"!Hello World!\". Remain [ %d ].\n", mIsMain ? "MAIN " : "", roleName.getString(), outputs);
    if (addMessage.isEmpty() == false)
    {
        printf("\t>>> The additional message: %s.\n", addMessage.getString());
    }

    setRemainOutput(outputs);
    if (mIsMain && (outputs == 0))
    {
        TRACE_INFO("Main component triggers shutdown procedure");
        broadcastServiceUnavailable();
    }
}

void RemoteServicingComponent::requestClientShutdown(unsigned int clientID, const String & roleName)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestClientShutdown);

    TRACE_DBG("A client [ %s ] with ID [ %u ] notified shutdown.", roleName.getString(), clientID);
    NERemoteHelloWorld::ConnectionList & list = getConnectedClients();
    LISTPOS pos = list.firstPosition();

    for ( ; pos != NULL; pos = list.nextPosition(pos))
    {
        const NERemoteHelloWorld::sConnectedClient & client = list.getAt(pos);
        if (client.ccID == clientID)
        {
            TRACE_DBG("Client [ %s ] with ID [ %d ] in component [ %s ] is removed", roleName.getString(), clientID, getRoleName().getString());
            ASSERT(client.ccName == roleName);
            list.removeAt(pos);
            notifyConnectedClientsUpdated();
            break;
        }
    }

    ASSERT(pos != NULL);

    if (mIsMain && list.isEmpty() && getRemainOutput() == 0)
    {
        TRACE_INFO("All clients are set message to shutdown, all [ %d ] messages are output, going to shutdown application", NERemoteHelloWorld::MaxMessages);
        Application::signalAppQuit();
    }
}
