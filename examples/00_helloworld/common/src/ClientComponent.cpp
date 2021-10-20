/**
 * \file    common/src/ClientComponent.hpp
 * \brief   Implementation of the service client component.
 **/
#include "common/src/ClientComponent.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

Component * ClientComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ClientComponent(entry, owner);
}

void ClientComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ClientComponent::ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName )
    , HelloWorldClientBase  ( entry.mDependencyServices[0].mRoleName.getString(), owner )
{
}

bool ClientComponent::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    bool result = false;
    if ( HelloWorldClientBase::serviceConnected(isConnected, proxy) )
    {
        if (isConnected)
        {
            // Up from this part the client
            //  - can send requests
            //  - can subscribe on data or event.

            // call request
            requestHelloWorld( getRoleName() );
        }
        else
        {
            // Make cleanups, release subscription here.
            // Since we've lost connection, exit client
            Application::signalAppQuit();
        }
    }

    return result;
}

void ClientComponent::responseHelloWorld( bool success )
{
    printf("%s to output message.\n", success ? "succeeded" : "failed");

    // The client completed the job, set signal to exit application
    // sleep for a while before exit application.
    Thread::sleep(NECommon::WAIT_500_MILLISECONDS);
    Application::signalAppQuit();
}

void ClientComponent::requestHelloWorldFailed(NEService::eResultType FailureReason)
{
    // make error handling here.
    printf("Failed to execute request, retry again.");
    if (isConnected())
    {
        // the service is still connected, can re-send reqest.
        requestHelloWorld( getRoleName() );
    }
}
