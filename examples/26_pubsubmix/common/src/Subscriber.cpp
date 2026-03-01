/************************************************************************
 * \file        common/src/Subscriber.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is demonstration of manual unlbocking of the request.
 *              The service client declaration
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "common/src/Subscriber.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"
#include "common/src/PubSubDefs.hpp"

#include <algorithm>
#include <string_view>

DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_serviceConnected);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onStringOnChangeUpdate);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onIntegerAlwaysUpdate);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onServiceProviderStateUpdate);

Subscriber::Subscriber(const areg::DependencyEntry & entry, areg::Component & owner, int32_t position)
    : PubSubMixClientBase   ( entry, owner )
    , mOldInteger   ( {0, entry.mRoleName } )
    , mOldState     ( false )
    , mOldString    ( { pubsub::Invalid, entry.mRoleName } )
    , mCoordInt     ( { pubsub::CoordInteger.posX    , pubsub::CoordInteger.posY  + position * 3 })
    , mCoordStr     ( { pubsub::CoordString.posX     , pubsub::CoordString.posY   + position * 3 })
    , mCoordSep     ( { pubsub::CoordSeparator.posX  , pubsub::CoordSeparator.posY+ position * 3 })
{
    pubsub::CoordInfoMsg.posY = std::max(pubsub::CoordInfoMsg.posY + 1, pubsub::CoordSeparator.posY);
}

bool Subscriber::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_serviceConnected);
    PubSubMixClientBase::service_connected( status, proxy );

    LOG_DBG("Service connection with status [ %s ]. If connected assign on provider state change", areg::as_string(status));

    bool connected = areg::is_service_connected(status);
    notifyOnServiceProviderStateUpdate(connected);

    aregext::Console & console = aregext::Console::instance();
    console.lock_console();
    console.save_cursor_position();
    if (connected == false)
    {
        notifyOnStringOnChangeUpdate(false);
        notifyOnIntegerAlwaysUpdate(false);

    }

    if (proxy.stub_address().role_name() == pubsub::ContollerPublisher)
    {
        if (connected)
        {
            console.output_txt(pubsub::CoordStatus, pubsub::TxtConnected);
        }
        else
        {
            console.output_msg(pubsub::CoordStatus, pubsub::FormatDisconnect.data(), areg::as_string(status));
        }
    }

    console.output_txt(pubsub::CoordSeparator, pubsub::Separator);
    console.refresh_screen();
    console.restore_cursor_position();
    console.unlock_console();

    return true;
}

void Subscriber::onStringOnChangeUpdate(const PubSubMix::sString & StringOnChange, areg::DataState state)
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onStringOnChangeUpdate);
    aregext::Console & console = aregext::Console::instance();
    console.lock_console();
    console.save_cursor_position();
    if (state == areg::DataState::DataIsOK)
    {
        ASSERT(StringOnChange.name == mOldString.name);

        bool isChanged{ StringOnChange != mOldString };

        LOG_INFO("The [ %s ] names STRING (on change) data is OK, old is [ %s ], new [ %s ]"
                   , StringOnChange.name.as_string()
                   , mOldString.value.as_string()
                   , StringOnChange.value.as_string()
                   , isChanged ? "CHANGED" : "UNCHANGED");

        console.output_msg( mCoordStr
                          , pubsub::FormatString.data()
                          , StringOnChange.name.as_string()
                          , mOldString.value.as_string()
                          , StringOnChange.value.as_string()
                          , isChanged ? "CHANGED" : "UNCHANGED");

        mOldString = StringOnChange;
    }
    else
    {
        LOG_INFO("The [ %s ] names STRING is invalidated, old is [ %s ], new [ %s ]"
                   , StringOnChange.name.as_string()
                   , mOldString.value.as_string()
                   , StringOnChange.value.as_string());

        console.output_msg ( mCoordStr
                          , pubsub::FormatString.data()
                          , StringOnChange.name.is_empty() ? "[Unknown]" : StringOnChange.name.as_string()
                          , mOldString.value.as_string()
                          , pubsub::Invalid.data()
                          , "INVALIDATED");

        mOldString.value = pubsub::Invalid;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Publisher state is invalid, unsubscribe on data { StringOnChange } update");
            notifyOnStringOnChangeUpdate(false);
        }
    }

    console.refresh_screen();
    console.restore_cursor_position();
    console.unlock_console();
}

void Subscriber::onIntegerAlwaysUpdate(const PubSubMix::sInteger & IntegerAlways, areg::DataState state)
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onIntegerAlwaysUpdate);
    aregext::Console & console = aregext::Console::instance();
    console.lock_console();
    areg::String oldInt = mOldState ? areg::String::make_string(mOldInteger.value) : pubsub::Invalid;
    console.save_cursor_position();
    if (state == areg::DataState::DataIsOK)
    {
        bool isChanged = mOldInteger == IntegerAlways;
        LOG_INFO("The [ %s ] names INTEGER (Always) data is OK, old is [ %s ], new [ %u ], { %s }"
                   , IntegerAlways.name.as_string()
                   , oldInt.as_string()
                   , IntegerAlways.value
                   , isChanged ? "CHANGED" : "UNCHANGED");

        console.output_msg(  mCoordInt
                          , pubsub::FormatInteger.data()
                          , IntegerAlways.name.as_string()
                          , oldInt.as_string()
                          , areg::String::make_string(IntegerAlways.value).as_string()
                          , isChanged ? "CHANGED" : "UNCHANGED");

        mOldInteger = IntegerAlways;
        mOldState = true;
    }
    else
    {
        LOG_INFO("The [ %s ] names INTEGER is invalidated, old is [ %s ], new [ %s ]"
                   , IntegerAlways.name.as_string()
                   , oldInt.as_string()
                   , pubsub::Invalid.data());

        console.output_msg(  mCoordInt
                          , pubsub::FormatInteger.data()
                          , IntegerAlways.name.is_empty() ? "[Unknown]" : IntegerAlways.name.as_string()
                          , oldInt.as_string()
                          , pubsub::Invalid.data()
                          , "INVALIDATED");


        mOldInteger.value = 0;
        mOldState = false;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { IntegerAlways } update");
            notifyOnIntegerAlwaysUpdate(false);
        }
    }

    console.refresh_screen();
    console.restore_cursor_position();
    console.unlock_console();
}

void Subscriber::onServiceProviderStateUpdate(PubSubMix::RunState ServiceProviderState, areg::DataState state)
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onServiceProviderStateUpdate);
    if (state == areg::DataState::DataIsOK)
    {
        if (isIntegerAlwaysValid() == false)
        {
            LOG_DBG("The integer to update ALWAYS is not valid, subscribe on data");
            notifyOnIntegerAlwaysUpdate(true);
        }

        if (isStringOnChangeValid() == false)
        {
            LOG_DBG("The string to update ON CHANGE is not valid, subscribe on data");
            notifyOnStringOnChangeUpdate(true);
        }

        if (ServiceProviderState == PubSubMix::RunState::Shutdown)
        {
            notifyOnStringOnChangeUpdate(false);
            notifyOnIntegerAlwaysUpdate(false);
            areg::Application::signal_app_quit();
        }
    }
}

inline Subscriber & Subscriber::self()
{
    return (*this);
}
