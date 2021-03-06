/************************************************************************
 * \file        generated/src/MainRemoteServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "generated/src/MainRemoteServiceClient.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainRemoteServiceClient_broadcastServiceShutdown);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_MainRemoteServiceClient_serviceConnected);


MainRemoteServiceClient::MainRemoteServiceClient(const String & roleName, Component & owner, unsigned int timeout)
    : RemoteServiceClient   ( roleName, owner, timeout)
{
}

void MainRemoteServiceClient::broadcastServiceShutdown(void)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainRemoteServiceClient_broadcastServiceShutdown);
    RemoteRegistryClientBase::clearAllNotifications();
    SystemShutdownClientBase::clearAllNotifications();

    mTimer.stopTimer();
    mClient.crID = 0;

    // Application quit.
    Application::signalAppQuit();
}

bool MainRemoteServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_MainRemoteServiceClient_serviceConnected);

    if (RemoteServiceClient::serviceConnected(isConnected, proxy))
    {
        if (&proxy == SystemShutdownClientBase::getProxy())
        {
            TRACE_DBG("Client [ %p ]: Subscribe on service shutdown message", this);
            notifyOnBroadcastServiceShutdown(isConnected);
        }

        return true;
    }
    else
    {
        return false;
    }
}
