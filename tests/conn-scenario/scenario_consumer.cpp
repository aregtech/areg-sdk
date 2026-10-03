/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        conn-scenario/scenario_consumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the consumer of the connection scenario harness: checks the number
 *              and the content of every block and reports gaps, stalls and connection changes.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "conn-scenario/conn_scenario.hpp"

#include "areg/base/SharedBuffer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "examples/25_pubsub/services/PubSubConsumerBase.hpp"
#include "examples/32_pubmixed/services/MixedTrafficConsumerBase.hpp"

#include <cstdio>
#include <cstdlib>

using namespace scenario;

//////////////////////////////////////////////////////////////////////////
// Consumer
//////////////////////////////////////////////////////////////////////////

//!< Receives the numbered blocks and reports gaps, stalls, damaged blocks and connection changes.
class ScenarioConsumer final    : public    areg::Component
                                , protected MixedTrafficConsumerBase
                                , protected areg::TimerConsumer
{
public:
    ScenarioConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , MixedTrafficConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
        , areg::TimerConsumer       ( )
        , mTimer                    ( static_cast<areg::TimerConsumer &>(*this), "ConnScenarioReport" )
        , mExpected                 ( 0u )
        , mDown                     ( false )
        , mReceived                 ( 0u )
        , mLast                     ( 0u )
        , mLastAt                   ( 0u )
        , mMaxGapMs                 ( 0u )
        , mLost                     ( 0u )
        , mBad                      ( 0u )
    {
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        const bool result{ MixedTrafficConsumerBase::service_connected(status, proxy) };
        const bool connected{ areg::is_service_connected(status) };
        ::printf("CONN %llu %s %s\n", static_cast<unsigned long long>(now_ms())
                 , options().name.c_str(), connected ? "up" : "down");
        ::fflush(stdout);

        if (connected)
        {
            notify_on_broadcast_bulk_block(true);
            mDown = false;
            mTimer.start_timer(1'000u, areg::Timer::CONTINUOUSLY);
        }
        else
        {
            mTimer.stop_timer();
            mExpected = 0u;
            mDown = true;
            if (std::getenv("CONN_SCENARIO_UNSUBSCRIBE") != nullptr)
            {
                notify_on_broadcast_bulk_block(false);
            }
        }

        return result;
    }

    void broadcast_bulk_block(const areg::SharedBuffer & block, uint32_t producer) final
    {
        const uint64_t now{ now_ms() };
        const uint32_t seq{ producer };
        ++mReceived;

        if ((block.size_used() != options().bytes) || (check_block(block.buffer(), block.size_used(), seq) == false))
        {
            ++mBad;
            ::printf("BAD %llu %s seq=%u size=%u\n", static_cast<unsigned long long>(now)
                     , options().name.c_str(), seq, block.size_used());
        }

        if (mDown)
        {
            ::printf("LATE %llu %s seq=%u\n", static_cast<unsigned long long>(now), options().name.c_str(), seq);
        }

        if ((mExpected != 0u) && (seq != mExpected))
        {
            const uint32_t lost{ seq > mExpected ? seq - mExpected : 0u };
            mLost += lost;
            ::printf("GAP %llu %s expected=%u got=%u lost=%u\n", static_cast<unsigned long long>(now)
                     , options().name.c_str(), mExpected, seq, lost);
        }

        if (mLastAt != 0u)
        {
            const uint32_t gap{ static_cast<uint32_t>(now - mLastAt) };
            mMaxGapMs = gap > mMaxGapMs ? gap : mMaxGapMs;
            if (gap >= STALL_REPORT_MS)
            {
                ::printf("STALL %llu %s gap_ms=%u seq=%u\n", static_cast<unsigned long long>(now)
                         , options().name.c_str(), gap, seq);
            }
        }

        mExpected = seq + 1u;
        mLast = seq;
        mLastAt = now;
    }

    void response_pong(uint64_t /*stamp*/) final
    {
    }

    void process_timer(areg::Timer & /*timer*/) final
    {
        ::printf("C %llu %s recv=%llu last=%u maxgap_ms=%u lost=%llu bad=%llu\n"
                 , static_cast<unsigned long long>(now_ms()), options().name.c_str()
                 , static_cast<unsigned long long>(mReceived), mLast, mMaxGapMs
                 , static_cast<unsigned long long>(mLost), static_cast<unsigned long long>(mBad));
        ::fflush(stdout);
        mMaxGapMs = 0u;
    }

private:
    areg::Timer mTimer;
    uint32_t    mExpected;
    bool        mDown;
    uint64_t    mReceived;
    uint32_t    mLast;
    uint64_t    mLastAt;
    uint32_t    mMaxGapMs;
    uint64_t    mLost;
    uint64_t    mBad;
};

//////////////////////////////////////////////////////////////////////////
// Attribute consumer
//////////////////////////////////////////////////////////////////////////

//!< Subscribes to the always-notified attribute and counts its updates and repeated values.
class AttributeConsumer final   : public    areg::Component
                                , protected PubSubConsumerBase
                                , protected areg::TimerConsumer
{
public:
    AttributeConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component       ( entry, owner )
        , PubSubConsumerBase    ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
        , areg::TimerConsumer   ( )
        , mTimer                ( static_cast<areg::TimerConsumer &>(*this), "ConnScenarioAttrReport" )
        , mUpdates              ( 0u )
        , mRepeats              ( 0u )
        , mLast                 ( 0u )
    {
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        const bool result{ PubSubConsumerBase::service_connected(status, proxy) };
        const bool connected{ areg::is_service_connected(status) };
        ::printf("ACONN %llu %s %s\n", static_cast<unsigned long long>(now_ms())
                 , options().name.c_str(), connected ? "up" : "down");
        ::fflush(stdout);

        if (connected)
        {
            notify_on_integer_always_update(true);
            mTimer.start_timer(1'000u, areg::Timer::CONTINUOUSLY);
        }
        else
        {
            mTimer.stop_timer();
            if (std::getenv("CONN_SCENARIO_UNSUBSCRIBE") != nullptr)
            {
                notify_on_integer_always_update(false);
            }
        }

        return result;
    }

    void on_integer_always_update(uint32_t IntegerAlways, areg::DataState state) final
    {
        if (state != areg::DataState::DataIsOK)
            return;

        ++mUpdates;
        mRepeats += (IntegerAlways == mLast) ? 1u : 0u;
        mLast = IntegerAlways;
    }

    void process_timer(areg::Timer & /*timer*/) final
    {
        ::printf("ATTR %llu %s updates=%u repeats=%u last=%u\n", static_cast<unsigned long long>(now_ms())
                 , options().name.c_str(), mUpdates, mRepeats, mLast);
        ::fflush(stdout);
    }

private:
    areg::Timer mTimer;
    uint32_t    mUpdates;
    uint32_t    mRepeats;
    uint32_t    mLast;
};

BEGIN_MODEL(CONSUMER_MODEL)
    BEGIN_REGISTER_THREAD("ConnScenarioConsumerThread")
        BEGIN_REGISTER_COMPONENT("ConnScenarioConsumer", ScenarioConsumer)
            REGISTER_DEPENDENCY(ROLE_PROVIDER)
        END_REGISTER_COMPONENT("ConnScenarioConsumer")
        BEGIN_REGISTER_COMPONENT("ConnScenarioAttributeConsumer", AttributeConsumer)
            REGISTER_DEPENDENCY(ROLE_ATTRIBUTE)
        END_REGISTER_COMPONENT("ConnScenarioAttributeConsumer")
    END_REGISTER_THREAD("ConnScenarioConsumerThread")
END_MODEL(CONSUMER_MODEL)

