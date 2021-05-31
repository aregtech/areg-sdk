/************************************************************************
 * \file        shareipcmix/src/private/LocalServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "shareipcmix/src/LocalServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_shutdownServiceIntrface);
DEF_TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_requestClientShutdown);

LocalServicingComponent::LocalServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName)
    , LocalHelloWorldStub   ( static_cast<Component &>(self()) )
    , mGnerateID            ( 0 )
{

}

LocalServicingComponent::~LocalServicingComponent(void)
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

void LocalServicingComponent::shutdownServiceIntrface ( Component & holder )
{
    TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_shutdownServiceIntrface);
    TRACE_WARN("Component [ %s ] shuts down service.", getRoleName().getString());

    LocalHelloWorldStub::shutdownServiceIntrface(holder);
}

void LocalServicingComponent::requestHelloWorld(const String & roleName, const String & addMessage /*= "" */)
{
    TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_requestHelloWorld);

    NELocalHelloWorld::ConnectionList & list = getConnectedClients();
    LISTPOS pos = list.firstPosition();
    NELocalHelloWorld::sConnectedClient cl;
    for ( ; pos != NULL; pos = list.nextPosition(pos))
    {
        const NELocalHelloWorld::sConnectedClient & client = list.getAt(pos);
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

    short outputs = getRemainOutput() - 1;
    setRemainOutput(outputs);
    TRACE_DBG("Remain  to output message [ %d ] in local servicing component [ %d ]", outputs );

    printf(">> LOCAL service [ %s ] client [ %s ]>: \"Hi there!\". Remain [ %d ].\n", roleName.getString(), getRoleName().getString(), outputs);
}

void LocalServicingComponent::requestClientShutdown(unsigned int clientID, const String & roleName)
{
    TRACE_SCOPE(examples_13_shareipcmix_LocalServicingComponent_requestClientShutdown);

    TRACE_DBG("A client [ %s ] with ID [ %u ] notified shutdown.", roleName.getString(), clientID);
    NELocalHelloWorld::ConnectionList & list = getConnectedClients();
    LISTPOS pos = list.firstPosition();

    for ( ; pos != NULL; pos = list.nextPosition(pos))
    {
        const NELocalHelloWorld::sConnectedClient & client = list.getAt(pos);
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
}
