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


DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient, process_event);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient, service_connected);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient, response_start_traffic_light);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient, response_stop_traffic_light);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_PowerControllerClient, on_run);

PowerControllerClient::PowerControllerClient(const char* roleName, areg::Component & owner)
    : PowerManagerConsumerBase        (roleName, owner)
    , areg::ThreadConsumer              ( )
    , IEPowerControllerEventConsumer( )

    , mConsole                      ( static_cast<areg::ThreadConsumer &>(self()), PowerControllerClient::ConsoleThreadName.data() )
{
}

void PowerControllerClient::response_start_traffic_light([[maybe_unused]] bool Success)
{
    LOG_SCOPE( 19_pubfsm_pubservice_src_PowerControllerClient, response_start_traffic_light );
    LOG_DBG("[ %s ] to start the traffic light controller", Success ? "SUCCEEDED" : "FAILED");
}

void PowerControllerClient::response_stop_traffic_light([[maybe_unused]] bool Success)
{
    LOG_SCOPE( 19_pubfsm_pubservice_src_PowerControllerClient, response_stop_traffic_light );
    LOG_DBG("[ %s ] to stop the traffic light controller", Success ? "SUCCEEDED" : "FAILED");
}

void PowerControllerClient::on_run()
{
    LOG_SCOPE( 19_pubfsm_pubservice_src_PowerControllerClient, on_run );

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

        if ((areg::compare_ignore_case<char, char>(command, "off") == areg::Ordering::Equal) || 
            (areg::compare_ignore_case<char, char>(command, "1") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to power OFF the traffic lights");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::PowerOff) );
        }
        else if ((areg::compare_ignore_case<char, char>(command, "on") == areg::Ordering::Equal) || 
                 (areg::compare_ignore_case<char, char>(command, "2") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to power ON the traffic lights");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::PowerOn) );
        }
        else if ((areg::compare_ignore_case<char, char>(command, "stop") == areg::Ordering::Equal) || 
                 (areg::compare_ignore_case<char, char>(command, "3") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to stop the traffic light controller.");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::StopLight) );
        }
        else if ((areg::compare_ignore_case<char, char>(command, "start") == areg::Ordering::Equal) || 
                 (areg::compare_ignore_case<char, char>(command, "4") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to start the traffic light controller.");
            PowerControllerEvent::send_event( PowerControllerEventData(PowerControllerEventData::Action::StartLight) );
        }
        else if ((areg::compare_ignore_case<char, char>(command, "quit") == areg::Ordering::Equal) || 
                 (areg::compare_ignore_case<char, char>(command, "q") == areg::Ordering::Equal) || 
                 (areg::compare_ignore_case<char, char>(command, "5") == areg::Ordering::Equal))
        {
            LOG_DBG("User requested command to quit the traffic light controller application.");
            loop = false;
        }

        printf("\n");
    } while (loop);

    printf("Quiting the Traffic Light Controller application ...\n");
    areg::Application::signal_quit();
}

void PowerControllerClient::process_event(const PowerControllerEventData & data)
{
    LOG_SCOPE( 19_pubfsm_pubservice_src_PowerControllerClient, process_event );
    LOG_DBG("The power controller client is going to process command [ %s ]", PowerControllerEventData::as_string(data.getAction()));

    switch (data.getAction())
    {
    case PowerControllerEventData::Action::PowerOff:
        request_power_off();
        break;

    case PowerControllerEventData::Action::PowerOn:
        request_power_on();
        break;

    case PowerControllerEventData::Action::StopLight:
        request_stop_traffic_light();
        break;

    case PowerControllerEventData::Action::StartLight:
        request_start_traffic_light();
        break;

    case PowerControllerEventData::Action::None:   // fall through
    default:
        break;
    }
}

bool PowerControllerClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( 19_pubfsm_pubservice_src_PowerControllerClient, service_connected );

    bool result = PowerManagerConsumerBase::service_connected( status, proxy );
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
