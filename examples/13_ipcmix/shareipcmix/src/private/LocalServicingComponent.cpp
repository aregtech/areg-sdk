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
