/**
 * \file    common/src/ClientComponent.hpp
 * \brief   Implementation of the service client component.
 **/
#include "common/src/ClientComponent.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

#include <iostream>

Component * ClientComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ClientComponent(entry, owner);
}

void ClientComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ClientComponent::ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( entry, owner )
    , HelloServiceClientBase( entry.mDependencyServices[0].mRoleName.getString(), owner )
{
}

bool ClientComponent::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    bool result{ false };
    if ( HelloServiceClientBase::serviceConnected(status, proxy) )
    {
        result = true;
        if (NEService::isServiceConnected(status))
        {
            // Up from this part the client can:
            //      a. call requests to run on the server side.
            //      b. subscribe on data update notification
            //      c. subscribe on broadcasts and responses.

            // call request to run on server side.
            requestHelloService( getRoleName() );
        }
        else
        {
            // No connection, make cleanups, release subscription here, signal to quit application.
            Application::signalAppQuit();
        }
    }

    return result;
}

void ClientComponent::responseHelloService( bool success )
{
    std::cout << (success ? "Succeeded" : "Failed") << " to output message." << std::endl;

    // Sleep for no reason! Do not do this in a real application.
    // It is done to give a chance to see an output message on the console.
    // Otherwise, the next line of code closes the application and you miss the message on console.
    Thread::sleep(NECommon::WAIT_1_SECOND);

    // The client completed the job, set signal to quit application
    Application::signalAppQuit();
}

void ClientComponent::requestHelloServiceFailed(NEService::eResultType FailureReason)
{
    // make error handling here.
    std::cerr << "Failed to execute request, retry again." << std::endl;

    // Try again, if connected
    if (isConnected())
    {
        // the service is still connected, and can resend the request.
        requestHelloService( getRoleName() );
    }
}
