/************************************************************************
 * \file        subscribermulti/src/SubscriberBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscribermulti/src/SubscriberBase.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/logging/GELog.h"
#include "subscribermulti/src/PubSubDefs.hpp"

DEF_LOG_SCOPE(example_27_pubsubmulti_subscribermulti_SubscriberBase_onStringOnChangeUpdate);
DEF_LOG_SCOPE(example_27_pubsubmulti_subscribermulti_SubscriberBase_onIntegerAlwaysUpdate);
DEF_LOG_SCOPE(example_27_pubsubmulti_subscribermulti_SubscriberBase_onServiceProviderStateUpdate);


SubscriberBase::SubscriberBase(const NERegistry::DependencyEntry & entry, Component & owner, NEMath::Coord coordInt, NEMath::Coord coordStr)
    : PubSubClientBase  ( entry, owner )
    , mCoordInteger     ( coordInt )
    , mCoordString      ( coordStr )
    , mOldInteger       ( 0 )
    , mOldState         ( false )
    , mOldString        ( pubsub::StrInvalid )
    , mIntEventCount    ( 0 )
    , mStrEventCount    ( 0 )
{
}

void SubscriberBase::onStringOnChangeUpdate(const String & StringOnChange, NEService::DataState state)
{
    LOG_SCOPE(example_27_pubsubmulti_subscribermulti_SubscriberBase_onStringOnChangeUpdate);
    ++ mStrEventCount;

    Console & console = Console::getInstance();
    if (state == NEService::DataState::DataIsOK)
    {
        LOG_DBG("The STRING (on change) data is OK, old is [ %s ], new [ %s ], event count [ %u ]", mOldString.as_string(), StringOnChange.as_string(), mStrEventCount);
        console.output_msg(mCoordString, "%s%s => %s { changed }, event count: %u"
                          , pubsub::TxtString.data()
                          , mOldString.as_string()
                          , StringOnChange.as_string()
                          , mStrEventCount);
        mOldString = StringOnChange;
    }
    else
    {
        LOG_INFO("The STRING (on change) have got invalidated, old value [ %s ]", mOldString.as_string());

        console.output_msg( mCoordString, "%s%s => INVALID { invalid }, event count: %u"
                         , pubsub::TxtString.data()
                         , mOldString.as_string()
                         , mStrEventCount);
        mOldString = pubsub::StrInvalid;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { StringOnChange } update");
            notifyOnStringOnChangeUpdate(false);
        }
    }

    console.refresh_screen();
}

void SubscriberBase::onIntegerAlwaysUpdate(uint32_t IntegerAlways, NEService::DataState state)
{
    LOG_SCOPE(example_27_pubsubmulti_subscribermulti_SubscriberBase_onIntegerAlwaysUpdate);
    ++ mIntEventCount;

    Console & console = Console::getInstance();
    String oldInt = mOldState ? String::makeString(mOldInteger) : pubsub::StrInvalid;
    if (state == NEService::DataState::DataIsOK)
    {
        LOG_DBG("The INTEGER (always) data is OK, old is [ %s ], new [ %u ]", oldInt.as_string(), IntegerAlways);
        console.output_msg( mCoordInteger, "%s%s => %u { %s }, event count: %u"
                         , pubsub::TxtInteger.data()
                         , oldInt.as_string()
                         , IntegerAlways
                         , (mOldState == false) || (IntegerAlways != mOldInteger) ? "changed" : "UNCHANGED"
                         , mIntEventCount);
        mOldInteger = IntegerAlways;
        mOldState = true;
    }
    else
    {
        LOG_DBG("The INTEGER (ALWAYS) have got invalidated, old value [ %s ]", oldInt.as_string());

        console.output_msg( mCoordInteger, "%s%s => INVALID { invalid }, event count %u"
                         , pubsub::TxtInteger.data()
                         , oldInt.as_string()
                         , mIntEventCount);
        mOldInteger = 0;
        mOldState = false;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { IntegerAlways } update");
            notifyOnIntegerAlwaysUpdate(false);
        }
    }

    console.refresh_screen();
}
