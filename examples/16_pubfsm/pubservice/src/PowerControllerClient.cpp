/************************************************************************
 * \file        pubservice/src/PowerControllerClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a Traffic Light controller client.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "pubservice/src/PowerControllerClient.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/Application.hpp"

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
#endif  // _WINDOWS


DEF_TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_processEvent);
DEF_TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_serviceConnected);
DEF_TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_responseStartTrafficLight);
DEF_TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_responseStopTrafficLight);
DEF_TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_onThreadRuns);

PowerControllerClient::PowerControllerClient(const char* roleName, Component & owner)
    : PowerManagerClientBase        (roleName, owner)
    , IEThreadConsumer              ( )
    , IEPowerControllerEventConsumer( )

    , mConsole                      ( static_cast<IEThreadConsumer &>(self()), PowerControllerClient::ConsoleThreadName.data() )
{
}

void PowerControllerClient::responseStartTrafficLight(bool Success)
{
    TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_responseStartTrafficLight);
    TRACE_DBG("[ %s ] to start the traffic light controller", Success ? "SUCCEEDED" : "FAILED");
}

void PowerControllerClient::responseStopTrafficLight(bool Success)
{
    TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_responseStopTrafficLight);
    TRACE_DBG("[ %s ] to stop the traffic light controller", Success ? "SUCCEEDED" : "FAILED");
}

void PowerControllerClient::onThreadRuns(void)
{
    TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_onThreadRuns);

    bool loop = true;

    do 
    {
        char command[32] = {0};

        printf("=========================================================\n");
        printf("Enter one of commands or type the command number \n");
        printf("\t1. Type \"off\" to power OFF the traffic light;\n");
        printf("\t2. Type \"on\" to power ON the traffic light;\n");
        printf("\t3. Type \"stop\" to stop the traffic light controlling;\n");
        printf("\t4. Type \"start\" to start the traffic light controling;\n");
        printf("\t5. Type \"quit\" or \"q\" to exit application;\n");
        printf("---------------------------------------------------------\n");
        printf("Type your command: ");

       if (MACRO_SCANF("%31s", command, 32) != 1)
        {
            // should never happen
            printf("\nERROR: Unexpectd command, quiting applicatio ...\n");
            break;

        }

        if ((NEString::compareFastIgnoreCase<char, char>(command, "off") == 0) || (NEString::compareFastIgnoreCase<char, char>(command, "1") == 0))
        {
            TRACE_DBG("User requested command to power OFF the traffic lights");
            PowerControllerEvent::sendEvent( PowerControllerEventData(PowerControllerEventData::eAction::ActionPowerOff) );
        }
        else if ((NEString::compareFastIgnoreCase<char, char>(command, "on") == 0) || (NEString::compareFastIgnoreCase<char, char>(command, "2") == 0))
        {
            TRACE_DBG("User requested command to power ON the traffic lights");
            PowerControllerEvent::sendEvent( PowerControllerEventData(PowerControllerEventData::eAction::ActionPowerOn) );
        }
        else if ((NEString::compareFastIgnoreCase<char, char>(command, "stop") == 0) || (NEString::compareFastIgnoreCase<char, char>(command, "3") == 0))
        {
            TRACE_DBG("User requested command to stop the traffic light controller.");
            PowerControllerEvent::sendEvent( PowerControllerEventData(PowerControllerEventData::eAction::ActionStopLight) );
        }
        else if ((NEString::compareFastIgnoreCase<char, char>(command, "start") == 0) || (NEString::compareFastIgnoreCase<char, char>(command, "4") == 0))
        {
            TRACE_DBG("User requested command to start the traffic light controller.");
            PowerControllerEvent::sendEvent( PowerControllerEventData(PowerControllerEventData::eAction::ActionStartLight) );
        }
        else if ((NEString::compareFastIgnoreCase<char, char>(command, "quit") == 0) || (NEString::compareFastIgnoreCase<char, char>(command, "q") == 0) || (NEString::compareFastIgnoreCase<char, char>(command, "5") == 0))
        {
            TRACE_DBG("User requested command to quit the traffic light controller application.");
            loop = false;
        }

        printf("\n");
    } while (loop);

    printf("Quiting the Traffic Light Controller application ...");
    Application::signalAppQuit();
}

void PowerControllerClient::processEvent(const PowerControllerEventData & data)
{
    TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_processEvent);
    TRACE_DBG("The power controller client is going to process command [ %s ]", PowerControllerEventData::getString(data.getAction()));

    switch (data.getAction())
    {
    case PowerControllerEventData::eAction::ActionPowerOff:
        requestPowerOff();
        break;

    case PowerControllerEventData::eAction::ActionPowerOn:
        requestPowerOn();
        break;

    case PowerControllerEventData::eAction::ActionStopLight:
        requestStopTrafficLight();
        break;

    case PowerControllerEventData::eAction::ActionStartLight:
        requestStartTrafficLight();
        break;

    case PowerControllerEventData::eAction::NoAction:   // fall through
    default:
        break;
    }
}

bool PowerControllerClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(16_pubfsm_pubservice_src_PowerControllerClient_serviceConnected);

    bool result = false;
    if (PowerManagerClientBase::serviceConnected(isConnected, proxy))
    {
        if (isConnected)
        {
            PowerControllerEvent::addListener( static_cast<IEPowerControllerEventConsumer &>(self()), proxy.getProxyDispatcherThread() );

            mConsole.createThread(NECommon::WAIT_INFINITE);
        }
        else
        {
            mConsole.destroyThread(NECommon::WAIT_INFINITE);

            PowerControllerEvent::removeListener( static_cast<IEPowerControllerEventConsumer &>(self()) );
        }

        result = true;
    }

    return result;
}
