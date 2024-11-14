/************************************************************************
 * \file        subscribermulti/src/SubscriberBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscribermulti/src/SubscriberBase.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/logging/GELog.h"
#include "subscribermulti/src/NECommon.hpp"

DEF_LOG_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberBase_onStringOnChangeUpdate);
DEF_LOG_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberBase_onIntegerAlwaysUpdate);
DEF_LOG_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberBase_onServiceProviderStateUpdate);


SubscriberBase::SubscriberBase(const NERegistry::DependencyEntry & entry, Component & owner, NEMath::sCoord coordInt, NEMath::sCoord coordStr)
    : PubSubClientBase  ( entry, owner )
    , mCoordInteger     ( coordInt )
    , mCoordString      ( coordStr )
    , mOldInteger       ( 0 )
    , mOldState         ( false )
    , mOldString        ( NECommon::StrInvalid )
    , mIntEventCount    ( 0 )
    , mStrEventCount    ( 0 )
{
}

void SubscriberBase::onStringOnChangeUpdate(const String & StringOnChange, NEService::eDataStateType state)
{
    LOG_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberBase_onStringOnChangeUpdate);
    ++ mStrEventCount;

    Console & console = Console::getInstance();
    if (state == NEService::eDataStateType::DataIsOK)
    {
        LOG_DBG("The STRING (on change) data is OK, old is [ %s ], new [ %s ], event count [ %u ]", mOldString.getString(), StringOnChange.getString(), mStrEventCount);
        console.outputMsg(mCoordString, "%s%s => %s { changed }, event count: %u"
                          , NECommon::TxtString.data()
                          , mOldString.getString()
                          , StringOnChange.getString()
                          , mStrEventCount);
        mOldString = StringOnChange;
    }
    else
    {
        LOG_INFO("The STRING (on change) have got invalidated, old value [ %s ]", mOldString.getString());

        console.outputMsg( mCoordString, "%s%s => INVALID { invalid }, event count: %u"
                         , NECommon::TxtString.data()
                         , mOldString.getString()
                         , mStrEventCount);
        mOldString = NECommon::StrInvalid;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { StringOnChange } update");
            notifyOnStringOnChangeUpdate(false);
        }
    }

    console.refreshScreen();
}

void SubscriberBase::onIntegerAlwaysUpdate(unsigned int IntegerAlways, NEService::eDataStateType state)
{
    LOG_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberBase_onIntegerAlwaysUpdate);
    ++ mIntEventCount;

    Console & console = Console::getInstance();
    String oldInt = mOldState ? String::makeString(mOldInteger) : NECommon::StrInvalid;
    if (state == NEService::eDataStateType::DataIsOK)
    {
        LOG_DBG("The INTEGER (always) data is OK, old is [ %s ], new [ %u ]", oldInt.getString(), IntegerAlways);
        console.outputMsg( mCoordInteger, "%s%s => %u { %s }, event count: %u"
                         , NECommon::TxtInteger.data()
                         , oldInt.getString()
                         , IntegerAlways
                         , (mOldState == false) || (IntegerAlways != mOldInteger) ? "changed" : "UNCHANGED"
                         , mIntEventCount);
        mOldInteger = IntegerAlways;
        mOldState = true;
    }
    else
    {
        LOG_DBG("The INTEGER (ALWAYS) have got invalidated, old value [ %s ]", oldInt.getString());

        console.outputMsg( mCoordInteger, "%s%s => INVALID { invalid }, event count %u"
                         , NECommon::TxtInteger.data()
                         , oldInt.getString()
                         , mIntEventCount);
        mOldInteger = 0;
        mOldState = false;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { IntegerAlways } update");
            notifyOnIntegerAlwaysUpdate(false);
        }
    }

    console.refreshScreen();
}
