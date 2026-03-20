/************************************************************************
 * \file        subscriber/src/Subscriber.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscriber/src/Subscriber.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"

#include <string_view>

DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_service_connected);
DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_on_string_on_change_update);
DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_on_integer_always_update);
DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_on_service_provider_state_update);

namespace
{
    constexpr std::string_view  _title          { "PubSub features, Subscriber part..."};
    constexpr std::string_view  _separator      { "--------------------------------------------" };

    constexpr std::string_view  _invalid        { "INVALID" };

    constexpr std::string_view  _txtInteger     { "Integer update (Always) .: "};
    constexpr std::string_view  _txtString      { "String update (OnChange) : "};

    constexpr std::string_view  _txtConnected   { "Connected to PubSub service" };
    constexpr std::string_view  _fmtDisconnected{ "Connected to PubSub service with status [ %s ]" };

    constexpr areg::ext::Console::Coord    _coordTitle     { 0, 1 };
    constexpr areg::ext::Console::Coord    _coordSubtitle  { 0, 2 };
    constexpr areg::ext::Console::Coord    _coordStatus    { 0, 3 };

    constexpr areg::ext::Console::Coord    _coordInteger   { 0, 5 };
    constexpr areg::ext::Console::Coord    _coordString    { 0, 6 };
}

Subscriber::Subscriber( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         ( entry, owner )
    , PubSubConsumerBase  ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()) )
    , mOldInteger       ( 0 )
    , mOldState         ( false )
    , mOldString        (_invalid )
{
}

bool Subscriber::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_service_connected);
    PubSubConsumerBase::service_connected( status, proxy );

    LOG_DBG("Service connection with status [ %s ]. If connected assign on provider state change", areg::as_string(status));

    bool connected = areg::is_service_connected(status);
    notify_on_service_provider_state_update(connected);

    areg::ext::Console & console = areg::ext::Console::instance();

    if (connected == false)
    {
        notify_on_string_on_change_update(false);
        notify_on_integer_always_update(false);

        console.output_msg(_coordStatus, _fmtDisconnected.data(), areg::as_string(status));
    }
    else
    {
        console.clear_screen();
        console.output_txt(_coordTitle, _title);
        console.output_txt(_coordSubtitle, _separator);
        console.output_txt(_coordStatus, _txtConnected);
    }

    console.refresh_screen();

    return true;
}

void Subscriber::on_string_on_change_update(const areg::String & StringOnChange, areg::DataState state)
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_on_string_on_change_update);
    areg::ext::Console & console = areg::ext::Console::instance();
    if (state == areg::DataState::DataIsOK)
    {
        LOG_DBG("The STRING (on change) data is OK, old is [ %s ], new [ %s ]", mOldString.as_string(), StringOnChange.as_string());
        console.output_msg(_coordString, "%s%s => %s { changed }", _txtString.data(), mOldString.as_string(), StringOnChange.as_string());
        mOldString = StringOnChange;
    }
    else
    {
        LOG_INFO("The STRING (on change) have got invalidated, old value [ %s ]", mOldString.as_string());

        console.output_msg(_coordString, "%s%s => INVALID { invalid }", _txtString.data(), mOldString.as_string());
        mOldString = _invalid;

        if (!is_service_provider_state_valid())
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { StringOnChange } update");
            notify_on_string_on_change_update(false);
        }
    }

    console.refresh_screen();
}

void Subscriber::on_integer_always_update(uint32_t IntegerAlways, areg::DataState state)
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_on_integer_always_update);
    areg::ext::Console & console = areg::ext::Console::instance();
    areg::String oldInt = mOldState ? areg::String::make_string(mOldInteger) : _invalid;
    if (state == areg::DataState::DataIsOK)
    {
        LOG_DBG("The INTEGER (always) data is OK, old is [ %s ], new [ %u ]", oldInt.as_string(), IntegerAlways);
        console.output_msg(_coordInteger, "%s%s => %u { %s }"
                          , _txtInteger.data()
                          , oldInt.as_string()
                          , IntegerAlways
                          , (mOldState == false) || (IntegerAlways != mOldInteger) ? "changed" : "UNCHANGED");
        mOldInteger = IntegerAlways;
        mOldState = true;
    }
    else
    {
        LOG_DBG("The INTEGER (ALWAYS) have got invalidated, old value [ %s ]", oldInt.as_string());

        console.output_msg(_coordInteger, "%s%s => INVALID { invalid }", _txtInteger.data(), oldInt.as_string());
        mOldInteger = 0;
        mOldState = false;

        if (!is_service_provider_state_valid())
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { IntegerAlways } update");
            notify_on_integer_always_update(false);
        }
    }

    console.refresh_screen();
}

void Subscriber::on_service_provider_state_update(PubSub::RunState ServiceProviderState, areg::DataState state)
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_on_service_provider_state_update);
    if (state == areg::DataState::DataIsOK)
    {
        if (!is_integer_always_valid())
        {
            LOG_DBG("The integer to update ALWAYS is not valid, subscribe on data");
            notify_on_integer_always_update(true);
        }

        if (!is_string_on_change_valid())
        {
            LOG_DBG("The string to update ON CHANGE is not valid, subscribe on data");
            notify_on_string_on_change_update(true);
        }

        if (ServiceProviderState == PubSub::RunState::Shutdown)
        {
            notify_on_string_on_change_update(false);
            notify_on_integer_always_update(false);
            areg::Application::signal_quit();
        }
    }
}

inline Subscriber & Subscriber::self()
{
    return (*this);
}
