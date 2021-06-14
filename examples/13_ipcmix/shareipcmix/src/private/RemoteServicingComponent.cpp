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
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestRegister);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestUnregister);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestHelloWorld);

RemoteServicingComponent::RemoteServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName                        )
    , RemoteRegistryStub    ( static_cast<Component &>(self())              )
    , mGnerateID            ( 0                                             )
{
}

RemoteServicingComponent::~RemoteServicingComponent(void)
{
}

void RemoteServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_startupServiceInterface);
    TRACE_DBG("Component [ %s ] set maximum allowed output messages [ %d ]", getRoleName().getString(), NERemoteRegistry::MaximumOutputs);

    RemoteRegistryStub::startupServiceInterface(holder);

    setRegistryList(NERemoteRegistry::ListRegistry());
    setRemainOutputs(NERemoteRegistry::MaximumOutputs);
}

void RemoteServicingComponent::requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process )
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestRegister);
    TRACE_DBG("Received request to register client [ %s ] with service address [ %s ] and owner thread [ %s ] of process [ %s ]"
                    , name.getString()
                    , ServiceAddress::convAddressToPath(service).getString()
                    , thread.getString()
                    , process.getString());

    NERemoteRegistry::sClientRegister newClient;

    NERemoteRegistry::ListRegistry & list = getRegistryList();
    LISTPOS pos = list.firstPosition();
    for ( ; pos != NULL; pos = list.nextPosition(pos) )
    {
        const NERemoteRegistry::sClientRegister & client = list.getAt(pos);
        if ( (client.crName == name) && (client.crService == service) && (client.crThread == thread))
        {
            TRACE_DBG("Found registered client [ %s ], ignoring new registration", name.getString());
            newClient = client;
            break;
        }
    }

    if (pos == NULL)
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
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestUnregister);
    TRACE_DBG("The client [ %s ] with registered ID [ %u ] requested unregister.", client.crName.getString(), client.crID);

    NERemoteRegistry::ListRegistry & list = getRegistryList();
    for (LISTPOS pos = list.firstPosition(); pos != NULL; pos = list.nextPosition(pos))
    {
        const NERemoteRegistry::sClientRegister & entry = list.getAt(pos);
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
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServicingComponent_requestHelloWorld);

    bool succeeded = false;

    const NERemoteRegistry::ListRegistry & list = getRegistryList();
    LISTPOS pos = list.firstPosition();
    for ( ; pos != NULL; pos = list.nextPosition(pos))
    {
        const NERemoteRegistry::sClientRegister & entry = list.getAt(pos);
        if (entry.crID == clientID)
        {
            break;
        }
    }

    if ( pos != NULL )
    {
        unsigned int outputs   = getRemainOutputs();
        printf(">>> REMOTE client [ %s ] says \"!Hello World!\". Remain [ %d ].\n", list.getAt(pos).crName.getString(), outputs);
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
