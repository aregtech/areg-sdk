/************************************************************************
 * \file        common/src/Subscriber.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
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
#include "common/src/NECommon.hpp"

#include <string_view>

DEF_LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_serviceConnected);
DEF_LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_onStringOnChangeUpdate);
DEF_LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_onIntegerAlwaysUpdate);
DEF_LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_onServiceProviderStateUpdate);

Subscriber::Subscriber(const NERegistry::DependencyEntry & entry, Component & owner, int position)
    : PubSubMixClientBase   ( entry, owner )
    , mOldInteger   ( {0, entry.mRoleName } )
    , mOldState     ( false )
    , mOldString    ( { NECommon::Invalid, entry.mRoleName } )
    , mCoordInt     ( { NECommon::CoordInteger.posX    , NECommon::CoordInteger.posY  + position * 3 })
    , mCoordStr     ( { NECommon::CoordString.posX     , NECommon::CoordString.posY   + position * 3 })
    , mCoordSep     ( { NECommon::CoordSeparator.posX  , NECommon::CoordSeparator.posY+ position * 3 })
{
    NECommon::CoordInfoMsg.posY = MACRO_MAX(NECommon::CoordInfoMsg.posY + 1, NECommon::CoordSeparator.posY);
}

bool Subscriber::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_serviceConnected);
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

    if (proxy.getStubAddress().getRoleName() == NECommon::ContollerPublisher)
    {
        if (connected)
        {
            console.outputTxt(NECommon::CoordStatus, NECommon::TxtConnected);
        }
        else
        {
            console.outputMsg(NECommon::CoordStatus, NECommon::FormatDisconnect.data(), NEService::getString(status));
        }
    }

    console.outputTxt(NECommon::CoordSeparator, NECommon::Separator);
    console.refreshScreen();
    console.restoreCursorPosition();
    console.unlockConsole();

    return true;
}

void Subscriber::onStringOnChangeUpdate(const NEPubSubMix::sString & StringOnChange, NEService::eDataStateType state)
{
    LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_onStringOnChangeUpdate);
    Console & console = Console::getInstance();
    console.lockConsole();
    console.saveCursorPosition();
    if (state == NEService::eDataStateType::DataIsOK)
    {
        ASSERT(StringOnChange.name == mOldString.name);

        bool isChanged{ StringOnChange != mOldString };

        LOG_INFO("The [ %s ] names STRING (on change) data is OK, old is [ %s ], new [ %s ]"
                   , StringOnChange.name.getString()
                   , mOldString.value.getString()
                   , StringOnChange.value.getString()
                   , isChanged ? "CHANGED" : "UNCHANGED");

        console.outputMsg( mCoordStr
                          , NECommon::FormatString.data()
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
                          , NECommon::FormatString.data()
                          , StringOnChange.name.isEmpty() ? "[Unknown]" : StringOnChange.name.getString()
                          , mOldString.value.getString()
                          , NECommon::Invalid.data()
                          , "INVALIDATED");

        mOldString.value = NECommon::Invalid;

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

void Subscriber::onIntegerAlwaysUpdate(const NEPubSubMix::sInteger & IntegerAlways, NEService::eDataStateType state)
{
    LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_onIntegerAlwaysUpdate);
    Console & console = Console::getInstance();
    console.lockConsole();
    String oldInt = mOldState ? String::makeString(mOldInteger.value) : NECommon::Invalid;
    console.saveCursorPosition();
    if (state == NEService::eDataStateType::DataIsOK)
    {
        bool isChanged = mOldInteger == IntegerAlways;
        LOG_INFO("The [ %s ] names INTEGER (Always) data is OK, old is [ %s ], new [ %u ], { %s }"
                   , IntegerAlways.name.getString()
                   , oldInt.getString()
                   , IntegerAlways.value
                   , isChanged ? "CHANGED" : "UNCHANGED");

        console.outputMsg(  mCoordInt
                          , NECommon::FormatInteger.data()
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
                   , NECommon::Invalid.data());

        console.outputMsg(  mCoordInt
                          , NECommon::FormatInteger.data()
                          , IntegerAlways.name.isEmpty() ? "[Unknown]" : IntegerAlways.name.getString()
                          , oldInt.getString()
                          , NECommon::Invalid.data()
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

void Subscriber::onServiceProviderStateUpdate(NEPubSubMix::eServiceState ServiceProviderState, NEService::eDataStateType state)
{
    LOG_SCOPE(examples_23_pubsubmix_common_Subscriber_onServiceProviderStateUpdate);
    if (state == NEService::eDataStateType::DataIsOK)
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

        if (ServiceProviderState == NEPubSubMix::eServiceState::Shutdown)
        {
            notifyOnStringOnChangeUpdate(false);
            notifyOnIntegerAlwaysUpdate(false);
            Application::signalAppQuit();
        }
    }
}

inline Subscriber & Subscriber::self(void)
{
    return (*this);
}
