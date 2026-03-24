/************************************************************************
 * \file        common/src/Publisher.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is demonstration of data PubSub (Publish / Subscribe) features.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "common/src/Publisher.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"

//////////////////////////////////////////////////////////////////////////
// Log scopes
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(examples_26_pubsubmix_common_Publisher, consumer_connected);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Publisher, start);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Publisher, stop);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Publisher, invalidate);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Publisher, quit);
DEF_LOG_SCOPE(examples_26_pubsubmix_common_Publisher, process_timer);


namespace
{
    areg::String generateString(uint32_t seqNr)
    {
        areg::String result;
        return result.format("string_%u", seqNr);
    }
}

//////////////////////////////////////////////////////////////////////////
// Publisher class methods
//////////////////////////////////////////////////////////////////////////

Publisher::Publisher( areg::Component & owner )
    : PubSubMixProviderBase     ( owner )
    , areg::TimerConsumer   ( )

    , mTimerOnChange    (static_cast<areg::TimerConsumer &>(self()), owner.role_name() + "_OnUpdateTimer")
    , mTimerAlways      (static_cast<areg::TimerConsumer &>(self()), owner.role_name() + "_AlwaysTimer")
    , mClientCount      (0)

    , mSeqString        (0)
    , mCountString      (0)

    , mSeqInteger       (0)
    , mCountInteger     (0)

    , mLock             (false)
{
}

bool Publisher::consumer_connected(const areg::ProxyAddress & client, areg::ServiceConnectionState status)
{
    LOG_SCOPE( examples_26_pubsubmix_common_Publisher, consumer_connected);
    bool result = PubSubMixProviderBase::consumer_connected(client, status);

    LOG_DBG("Connection status [ %s ] of the consumer [ %s ]", areg::as_string(status), areg::ProxyAddress::to_path(client).as_string());
    mClientCount += (areg::is_service_connected(status) ? 1 : -1);
    LOG_DBG("There are [ %d ] connected service consumers", mClientCount);

    if (!is_service_provider_state_valid())
    {
        set_service_provider_state(PubSubMix::RunState::Uninitialized);
    }

    return result;
}

void Publisher::start()
{
    LOG_SCOPE( examples_26_pubsubmix_common_Publisher, start );

    areg::Lock lock(mLock);
    LOG_DBG("Requested to re-start the service run. Reset values and re-start timers, there are [ %d ] connected clients",  mClientCount);

    mTimerAlways.stop_timer();
    mTimerOnChange.stop_timer();

    set_service_provider_state(PubSubMix::RunState::Running);
    const areg::String & roleName = PubSubMixProviderBase::service_role();

    if (!is_integer_always_valid())
    {
        mSeqInteger = 0; 
        set_integer_always( { mSeqInteger, roleName } );
    }

    if (!is_string_on_change_valid())
    {
        mSeqString = 0;
        set_string_on_change( { generateString(mSeqString), roleName } );
    }

    mTimerAlways.start_timer(PubSubMix::TimeoutAlways, component_thread(), areg::Timer::CONTINUOUSLY);
    mTimerOnChange.start_timer(PubSubMix::TimeoutOnChange, component_thread(), areg::Timer::CONTINUOUSLY);
}

void Publisher::stop()
{
    LOG_SCOPE( examples_26_pubsubmix_common_Publisher, stop );

    areg::Lock lock(mLock);
    LOG_DBG("Stopped servicing, resets data, wait for further instructions. There are [ %d ] connected clients", mClientCount);

    mTimerAlways.stop_timer();
    mTimerOnChange.stop_timer();

    set_service_provider_state(PubSubMix::RunState::Stopped);
}

void Publisher::invalidate()
{
    LOG_SCOPE( examples_26_pubsubmix_common_Publisher, invalidate );

    areg::Lock lock(mLock);
    LOG_DBG("Invalidating all data. There are [ %d ] connected clients", mClientCount);

    mCountString = 0;
    mCountInteger = 0;
    mSeqString = 0;
    mSeqInteger = 0;

    set_service_provider_state(PubSubMix::RunState::Uninitialized);

    invalidate_integer_always();
    invalidate_string_on_change();
}

void Publisher::quit()
{
    LOG_SCOPE( examples_26_pubsubmix_common_Publisher, quit );

    areg::Lock lock(mLock);
    LOG_DBG("Requested to quit.There are[% d] connected clients", mClientCount);

    mTimerAlways.stop_timer();
    mTimerOnChange.stop_timer();

    set_service_provider_state(PubSubMix::RunState::Shutdown);
    areg::Application::signal_quit();
}

void Publisher::process_timer(areg::Timer & timer)
{
    LOG_SCOPE( examples_26_pubsubmix_common_Publisher, process_timer );

    const areg::String roleName = service_role();

    if (&timer == &mTimerAlways)
    {
        areg::Lock lock(mLock);
        if (++ mCountInteger > PubSubMix::CycleAlways)
        {
            ++ mSeqInteger;
            mCountInteger = 0;
        }

        LOG_DBG("Timer \'Update Always\' has expired, Integer sequence number is [ %u ], the data should be updated", mSeqInteger);
        set_integer_always({ mSeqInteger, roleName });
    }
    else if (&timer == &mTimerOnChange)
    {
        areg::Lock lock(mLock);
        if (++ mCountString > PubSubMix::CycleAlways)
        {
            ++ mSeqString;
            mCountString = 0;
        }

        areg::String data(generateString(mSeqString));
        LOG_DBG("Timer \'Update OnChange\' has expired, String is [ %s ], the data should be updated only on update", data.as_string());
        set_string_on_change({ data, roleName });
    }
    else
    {
        ASSERT(false);
    }
}

inline Publisher & Publisher::self()
{
    return (*this);
}
