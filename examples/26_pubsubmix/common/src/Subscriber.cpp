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
#include "areg/logging/GELog.h"
#include "aregextend/console/Console.hpp"
#include "common/src/PubSubDefs.hpp"

#include <algorithm>
#include <string_view>

DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_serviceConnected);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onStringOnChangeUpdate);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onIntegerAlwaysUpdate);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onServiceProviderStateUpdate);

Subscriber::Subscriber(const NERegistry::DependencyEntry & entry, Component & owner, int32_t position)
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

bool Subscriber::serviceConnected( NEService::ServiceConnectionState status, ProxyBase & proxy )
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_serviceConnected);
    PubSubMixClientBase::serviceConnected( status, proxy );

    LOG_DBG("Service connection with status [ %s ]. If connected assign on provider state change", NEService::getString(status));

    bool connected = NEService::isServiceConnected(status);
    notifyOnServiceProviderStateUpdate(connected);

    Console & console = Console::getInstance();
    console.lockConsole();
    console.saveCursorPosition();
    if (connected == false)
    {
        notifyOnStringOnChangeUpdate(false);
        notifyOnIntegerAlwaysUpdate(false);

    }

    if (proxy.getStubAddress().getRoleName() == pubsub::ContollerPublisher)
    {
        if (connected)
        {
            console.outputTxt(pubsub::CoordStatus, pubsub::TxtConnected);
        }
        else
        {
            console.outputMsg(pubsub::CoordStatus, pubsub::FormatDisconnect.data(), NEService::getString(status));
        }
    }

    console.outputTxt(pubsub::CoordSeparator, pubsub::Separator);
    console.refreshScreen();
    console.restoreCursorPosition();
    console.unlockConsole();

    return true;
}

void Subscriber::onStringOnChangeUpdate(const PubSubMix::sString & StringOnChange, NEService::DataState state)
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onStringOnChangeUpdate);
    Console & console = Console::getInstance();
    console.lockConsole();
    console.saveCursorPosition();
    if (state == NEService::DataState::DataIsOK)
    {
        ASSERT(StringOnChange.name == mOldString.name);

        bool isChanged{ StringOnChange != mOldString };

        LOG_INFO("The [ %s ] names STRING (on change) data is OK, old is [ %s ], new [ %s ]"
                   , StringOnChange.name.getString()
                   , mOldString.value.getString()
                   , StringOnChange.value.getString()
                   , isChanged ? "CHANGED" : "UNCHANGED");

        console.outputMsg( mCoordStr
                          , pubsub::FormatString.data()
                          , StringOnChange.name.getString()
                          , mOldString.value.getString()
                          , StringOnChange.value.getString()
                          , isChanged ? "CHANGED" : "UNCHANGED");

        mOldString = StringOnChange;
    }
    else
    {
        LOG_INFO("The [ %s ] names STRING is invalidated, old is [ %s ], new [ %s ]"
                   , StringOnChange.name.getString()
                   , mOldString.value.getString()
                   , StringOnChange.value.getString());

        console.outputMsg ( mCoordStr
                          , pubsub::FormatString.data()
                          , StringOnChange.name.isEmpty() ? "[Unknown]" : StringOnChange.name.getString()
                          , mOldString.value.getString()
                          , pubsub::Invalid.data()
                          , "INVALIDATED");

        mOldString.value = pubsub::Invalid;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Publisher state is invalid, unsubscribe on data { StringOnChange } update");
            notifyOnStringOnChangeUpdate(false);
        }
    }

    console.refreshScreen();
    console.restoreCursorPosition();
    console.unlockConsole();
}

void Subscriber::onIntegerAlwaysUpdate(const PubSubMix::sInteger & IntegerAlways, NEService::DataState state)
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onIntegerAlwaysUpdate);
    Console & console = Console::getInstance();
    console.lockConsole();
    String oldInt = mOldState ? String::makeString(mOldInteger.value) : pubsub::Invalid;
    console.saveCursorPosition();
    if (state == NEService::DataState::DataIsOK)
    {
        bool isChanged = mOldInteger == IntegerAlways;
        LOG_INFO("The [ %s ] names INTEGER (Always) data is OK, old is [ %s ], new [ %u ], { %s }"
                   , IntegerAlways.name.getString()
                   , oldInt.getString()
                   , IntegerAlways.value
                   , isChanged ? "CHANGED" : "UNCHANGED");

        console.outputMsg(  mCoordInt
                          , pubsub::FormatInteger.data()
                          , IntegerAlways.name.getString()
                          , oldInt.getString()
                          , String::makeString(IntegerAlways.value).getString()
                          , isChanged ? "CHANGED" : "UNCHANGED");

        mOldInteger = IntegerAlways;
        mOldState = true;
    }
    else
    {
        LOG_INFO("The [ %s ] names INTEGER is invalidated, old is [ %s ], new [ %s ]"
                   , IntegerAlways.name.getString()
                   , oldInt.getString()
                   , pubsub::Invalid.data());

        console.outputMsg(  mCoordInt
                          , pubsub::FormatInteger.data()
                          , IntegerAlways.name.isEmpty() ? "[Unknown]" : IntegerAlways.name.getString()
                          , oldInt.getString()
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

    console.refreshScreen();
    console.restoreCursorPosition();
    console.unlockConsole();
}

void Subscriber::onServiceProviderStateUpdate(PubSubMix::RunState ServiceProviderState, NEService::DataState state)
{
    LOG_SCOPE(examples_26_pubsubmix_common_Subscriber_onServiceProviderStateUpdate);
    if (state == NEService::DataState::DataIsOK)
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
            Application::signalAppQuit();
        }
    }
}

inline Subscriber & Subscriber::self()
{
    return (*this);
}
