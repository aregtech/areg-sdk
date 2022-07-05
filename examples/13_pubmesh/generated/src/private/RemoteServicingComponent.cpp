/************************************************************************
 * \file        generated/src/private/RemoteServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "generated/src/RemoteServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_requestRegister);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_requestUnregister);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_requestHelloWorld);

RemoteServicingComponent::RemoteServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName                        )
    , RemoteRegistryStub    ( static_cast<Component &>(self())              )
    , mGnerateID            ( 0                                             )
{
}

void RemoteServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_startupServiceInterface);
    TRACE_DBG("Component [ %s ] set maximum allowed output messages [ %d ]", getRoleName().getString(), NERemoteRegistry::MaximumOutputs);

    RemoteRegistryStub::startupServiceInterface(holder);

    setRegistryList(NERemoteRegistry::ListRegistry());
    setRemainOutputs(NERemoteRegistry::MaximumOutputs);
}

void RemoteServicingComponent::requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_requestRegister);
    TRACE_DBG("Received request to register client [ %s ] with service address [ %s ] and owner thread [ %s ] of process [ %s ]"
                    , name.getString()
                    , ServiceAddress::convAddressToPath(service).getString()
                    , thread.getString()
                    , process.getString());

    NERemoteRegistry::sClientRegister newClient;

    NERemoteRegistry::ListRegistry & list = getRegistryList();
    NERemoteRegistry::ListRegistry::LISTPOS pos = list.firstPosition();
    for ( ; list.isValidPosition(pos); pos = list.nextPosition(pos) )
    {
        const NERemoteRegistry::sClientRegister & client = list.valueAtPosition(pos);
        if ( (client.crName == name) && (client.crService == service) && (client.crThread == thread))
        {
            TRACE_DBG("Found registered client [ %s ], ignoring new registration", name.getString());
            newClient = client;
            break;
        }
    }

    if (list.isInvalidPosition(pos))
    {
        newClient.crID      = ++ mGnerateID;
        newClient.crName    = name;
        newClient.crService = service;
        newClient.crThread  = thread;
        newClient.crProcess = process;

        TRACE_DBG("Registered [ %u ] new client [ %s ] of service [ %s ] in thread [ %s ] of process [ %s ]"
                        , newClient.crID
                        , newClient.crName.getString()
                        , ServiceAddress::convAddressToPath(newClient.crService).getString()
                        , newClient.crThread.getString()
                        , newClient.crProcess.getString());

        list.pushLast(newClient);
        notifyRegistryListUpdated();
    }

    responseRegister(newClient);
}

void RemoteServicingComponent::requestUnregister( const NERemoteRegistry::sClientRegister & client )
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_requestUnregister);
    TRACE_DBG("The client [ %s ] with registered ID [ %u ] requested unregister.", client.crName.getString(), client.crID);

    NERemoteRegistry::ListRegistry & list = getRegistryList();
    for (NERemoteRegistry::ListRegistry::LISTPOS pos = list.firstPosition(); list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        const NERemoteRegistry::sClientRegister & entry = list.valueAtPosition(pos);
        if (entry == client)
        {
            list.removeAt(pos);
            notifyRegistryListUpdated();

            TRACE_DBG("Removed entry, there are still [ %d ] registered clients", list.getSize());
            break;
        }
    }
}

void RemoteServicingComponent::requestHelloWorld( unsigned int clientID, const String & addMessage )
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServicingComponent_requestHelloWorld);

    const NERemoteRegistry::ListRegistry & list = getRegistryList();
    NERemoteRegistry::ListRegistry::LISTPOS pos = list.firstPosition();
    for ( ; list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        const NERemoteRegistry::sClientRegister & entry = list.valueAtPosition(pos);
        if (entry.crID == clientID)
        {
            break;
        }
    }

    if ( list.isValidPosition(pos))
    {
        unsigned int outputs   = getRemainOutputs();
        printf(">>> REMOTE client [ %s ]:\n", list.valueAtPosition(pos).crName.getString());
        printf("    Says: \"!Hello World!\". Remain [ %d ].\n", outputs);
        if (addMessage.isEmpty() == false)
        {
            printf("\t>>> The additional message: %s.\n", addMessage.getString());
        }

        setRemainOutputs(-- outputs);

        responseHelloWorld(clientID);
    }
    else
    {
        printf("ERROR... ignoring output message!");
        responseHelloWorld(0);
    }
}
