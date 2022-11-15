/************************************************************************
 * \file        generated/src/private/MainServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of remote servicing 
 *              component to output messages and shutdown the system.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "generated/src/MainServicingComponent.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/trace/GETrace.h"
#include <stdlib.h>

DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_requestUnregister);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_requestRegister);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_clientConnected);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_processTimer);


MainServicingComponent::MainServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : RemoteServicingComponent  (entry, owner                       )
    , SystemShutdownStub        ( static_cast<Component &>(self())  )
    , IETimerConsumer           (                                   )

    , mTimer                    ( static_cast<IETimerConsumer &>(self()), "SystemShutdown")

{
    setServiceState(NESystemShutdown::eServiceState::ServiceInvalid);
}

void MainServicingComponent::requestUnregister(const NERemoteRegistry::sClientRegister & client)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_requestUnregister);

    RemoteServicingComponent::requestUnregister(client);
    if (getRegistryList().isEmpty())
    {
        setServiceState(NESystemShutdown::eServiceState::ServiceShutdown);
        broadcastServiceShutdown();
        mTimer.startTimer(NESystemShutdown::ShutdownWaitTimeout, Component::getMasterThread(), 1);
    }
}

void MainServicingComponent::requestRegister(const String & name, const ServiceAddress & service, const String & thread, const String & process)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_requestRegister);

    if (getServiceState() == NESystemShutdown::eServiceState::ServiceReady)
    {
        RemoteServicingComponent::requestRegister(name, service, thread, process);
    }
    else
    {
        TRACE_WARN("The service is not ready, cannot register client");
        responseRegister(NERemoteRegistry::sClientRegister());
    }
}

void MainServicingComponent::requestHelloWorld(unsigned int clientID)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_requestHelloWorld);
    if (getServiceState() == NESystemShutdown::eServiceState::ServiceReady)
    {
        RemoteServicingComponent::requestHelloWorld(clientID);

        if (getRemainOutputs() == 0)
        {
            TRACE_WARN("The maximum of output messages has been reached, the service is not available anymore, going to start shutdown procedure.");

            setServiceState(NESystemShutdown::eServiceState::ServiceUnavailable);
            broadcastServiceUnavailable();
        }
    }
    else
    {
        TRACE_WARN("Ignore output message from client [ %d ], the service is not ready", clientID);
        responseHelloWorld(0);
    }
}

void MainServicingComponent::startupServiceInterface(Component & holder)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_startupServiceInterface);
    RemoteServicingComponent::startupServiceInterface(holder);
    SystemShutdownStub::startupServiceInterface(holder);

    setServiceState(NESystemShutdown::eServiceState::ServiceReady);

    TRACE_INFO("The service is up and running, ready for servicing.");
}

void MainServicingComponent::clientConnected(const ProxyAddress & client, bool isConnected)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_clientConnected);
    TRACE_INFO("The client [ %s ] is [ %s ]", ProxyAddress::convAddressToPath(client).getString(), isConnected ? "CONNECTED" : "DISCONNECTED");

    if (isConnected == false)
    {
        TRACE_DBG("Searching service address [ %s ] in the thread [ %s ]"
                        , ServiceAddress::convAddressToPath(static_cast<const ServiceAddress &>(client)).getString()
                        , client.getThread().getString());

        NERemoteRegistry::ListRegistry & list = getRegistryList();
        TRACE_DBG("There are [ %d ] entries in the registry list.", list.getSize());

        for (NERemoteRegistry::ListRegistry::LISTPOS pos = list.firstPosition(); list.isValidPosition(pos); pos = list.nextPosition(pos) )
        {
            const NERemoteRegistry::sClientRegister & entry = list.valueAtPosition(pos);
            TRACE_DBG("Compare [ %s ] service and thread [ %s ]"
                            , ServiceAddress::convAddressToPath(entry.crService).getString()
                            , entry.crThread.getString());

            if ( (entry.crService == static_cast<const ServiceAddress &>(client)) && (entry.crThread == client.getThread()) )
            {
                TRACE_INFO("Found client [ %s ], remove from the list", entry.crName.getString());

                list.removeAt(pos);
                notifyRegistryListUpdated();
                break;
            }
        }


        if (list.isEmpty())
        {
            TRACE_DBG("The client list is empty, checking current state of service [ %s ]", NESystemShutdown::getString(getServiceState()));
            NESystemShutdown::eServiceState serviceState = getServiceState();
            if (serviceState == NESystemShutdown::eServiceState::ServiceUnavailable)
            {
                TRACE_DBG("The service is unavailable and client list is empty. Trigger timer.");

                setServiceState(NESystemShutdown::eServiceState::ServiceShutdown);
                broadcastServiceShutdown();
                mTimer.startTimer(NESystemShutdown::ShutdownWaitTimeout, Component::getMasterThread(), 1);
            }
            else if ((serviceState == NESystemShutdown::eServiceState::ServiceShutdown) && (mTimer.isActive() == false))
            {
                TRACE_WARN("The service state is shutdown. Quit the application");
                Application::signalAppQuit();
            }
        }
    }
}

void MainServicingComponent::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainServicingComponent_processTimer);

    if (&timer == &mTimer)
    {
        TRACE_DBG("The waiting timer [ %s ] has expired, quiting application", timer.getName().getString());
        Application::signalAppQuit();
    }
}
