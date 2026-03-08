/************************************************************************
 * \file        pubservice/src/PowerControllerClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a Traffic Light controller client.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "pubservice/src/PowerControllerClient.hpp"
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
#endif  // _WINDOWS


DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_processEvent);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_serviceConnected);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_responseStartTrafficLight);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_responseStopTrafficLight);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_onThreadRuns);

PowerControllerClient::PowerControllerClient(const char* roleName, areg::Component & owner)
    : PowerManagerClientBase        (roleName, owner)
    , areg::ThreadConsumer              ( )
    , IEPowerControllerEventConsumer( )

    , mConsole                      ( static_cast<areg::ThreadConsumer &>(self()), PowerControllerClient::ConsoleThreadName.data() )
{
}

void PowerControllerClient::responseStartTrafficLight(bool Success)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_responseStartTrafficLight);
    LOG_DBG("[ %s ] to start the traffic light controller", Success ? "SUCCEEDED" : "FAILED");
}

void PowerControllerClient::responseStopTrafficLight(bool Success)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_responseStopTrafficLight);
    LOG_DBG("[ %s ] to stop the traffic light controller", Success ? "SUCCEEDED" : "FAILED");
}

void PowerControllerClient::on_run()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_onThreadRuns);

    bool loop = true;

    do 
    {
        char command[32] = {0};

        printf("=========================================================\n");
        printf("Enter one of commands or type the command number \n");
        printf("\t1. Type \"off\" to power OFF the traffic light;\n");
        printf("\t2. Type \"on\" to power ON the traffic light;\n");
        printf("\t3. Type \"stop\" to stop the traffic light controlling;\n");
        printf("\t4. Type \"start\" to start the traffic light controlling;\n");
        printf("\t5. Type \"quit\" or \"q\" to exit application;\n");
        printf("---------------------------------------------------------\n");
        printf("Type your command: ");

       if (MACRO_SCANF("%31s", command, 32) != 1)
        {
            // should never happen
            printf("\nERROR: Unexpected command, quiting application ...\n");
            break;

        }

        if ((areg::compareIgnoreCase<char, char>(command, "off") == areg::Ordering::Equal) || 
            (areg::compareIgnoreCase<char, char>(command, "1") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to power OFF the traffic lights");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::PowerOff) );
        }
        else if ((areg::compareIgnoreCase<char, char>(command, "on") == areg::Ordering::Equal) || 
                 (areg::compareIgnoreCase<char, char>(command, "2") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to power ON the traffic lights");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::PowerOn) );
        }
        else if ((areg::compareIgnoreCase<char, char>(command, "stop") == areg::Ordering::Equal) || 
                 (areg::compareIgnoreCase<char, char>(command, "3") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to stop the traffic light controller.");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::StopLight) );
        }
        else if ((areg::compareIgnoreCase<char, char>(command, "start") == areg::Ordering::Equal) || 
                 (areg::compareIgnoreCase<char, char>(command, "4") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to start the traffic light controller.");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::StartLight) );
        }
        else if ((areg::compareIgnoreCase<char, char>(command, "quit") == areg::Ordering::Equal) || 
                 (areg::compareIgnoreCase<char, char>(command, "q") == areg::Ordering::Equal) || 
                 (areg::compareIgnoreCase<char, char>(command, "5") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to quit the traffic light controller application.");
            loop = false;
        }

        printf("\n");
    } while (loop);

    printf("Quiting the Traffic Light Controller application ...");
    areg::Application::signal_app_quit();
}

void PowerControllerClient::process_event(const PowerControllerEventData & data)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_processEvent);
    LOG_DBG("The power controller client is going to process command [ %s ]", PowerControllerEventData::as_string(data.getAction()));

    switch (data.getAction())
    {
    case PowerControllerEventData::Action::PowerOff:
        requestPowerOff();
        break;

    case PowerControllerEventData::Action::PowerOn:
        requestPowerOn();
        break;

    case PowerControllerEventData::Action::StopLight:
        requestStopTrafficLight();
        break;

    case PowerControllerEventData::Action::StartLight:
        requestStartTrafficLight();
        break;

    case PowerControllerEventData::Action::None:   // fall through
    default:
        break;
    }
}

bool PowerControllerClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient_serviceConnected);

    bool result = PowerManagerClientBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        LOG_DBG( "Adding PowerControllerEvent custom event listener to receive messages" );
        PowerControllerEvent::add_listener( static_cast<IEPowerControllerEventConsumer &>(self( )), proxy.proxy_dispatcher_thread( ) );
        mConsole.start( areg::WAIT_INFINITE );
    }
    else
    {
        LOG_DBG( "Remove listener and stop worker thread" );
        mConsole.shutdown( areg::WAIT_INFINITE );
        PowerControllerEvent::remove_listener( static_cast<IEPowerControllerEventConsumer &>(self( )) );
    }

    return result;
}
