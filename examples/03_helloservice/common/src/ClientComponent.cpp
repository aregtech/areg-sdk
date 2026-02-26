/**
 * \file    common/src/ClientComponent.hpp
 * \brief   Implementation of the service client component.
 **/
#include "common/src/ClientComponent.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

#include <iostream>

ClientComponent::ClientComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component             ( entry, owner )
    , HelloServiceClientBase( entry.mDependencyServices[0].mRoleName.getString(), owner )
{
}

bool ClientComponent::serviceConnected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result{ false };
    if ( HelloServiceClientBase::serviceConnected(status, proxy) )
    {
        result = true;
        if (areg::isServiceConnected(status))
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
    areg::Thread::sleep(areg::WAIT_1_SECOND);

    // The client completed the job, set signal to quit application
    Application::signalAppQuit();
}

void ClientComponent::requestHelloServiceFailed(areg::ResultType /* FailureReason */)
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
