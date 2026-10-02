/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        conn-scenario/scenario_provider.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the provider of the connection scenario harness: broadcasts numbered
 *              blocks at a fixed rate and reports how many it sent.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "conn-scenario/conn_scenario.hpp"

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "examples/25_pubsub/services/PubSubProviderBase.hpp"
#include "examples/32_pubmixed/services/MixedTrafficProviderBase.hpp"

#include <atomic>
#include <cstdio>
#include <vector>

using namespace scenario;

//////////////////////////////////////////////////////////////////////////
// Provider
//////////////////////////////////////////////////////////////////////////

//!< Broadcasts numbered blocks from its own thread and reports once a second.
class ScenarioProvider final    : public    areg::Component
                                , protected MixedTrafficProviderBase
                                , protected areg::ThreadConsumer
                                , protected areg::TimerConsumer
{
public:
    ScenarioProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , MixedTrafficProviderBase  ( static_cast<areg::Component &>(*this) )
        , areg::ThreadConsumer      ( )
        , areg::TimerConsumer       ( )
        , mThread                   ( static_cast<areg::ThreadConsumer &>(*this), "ConnScenarioSender" )
        , mTimer                    ( static_cast<areg::TimerConsumer &>(*this), "ConnScenarioReport" )
        , mSent                     ( 0u )
        , mQuit                     ( false )
    {
    }

protected:
    void startup_service_interface(areg::Component & holder) final
    {
        MixedTrafficProviderBase::startup_service_interface(holder);
        mTimer.start_timer(1'000u, component_thread(), areg::Timer::CONTINUOUSLY);
        mThread.start(areg::WAIT_INFINITE);
        ::printf("READY %llu provider bytes=%u gap_us=%u\n"
                 , static_cast<unsigned long long>(now_ms()), options().bytes, options().gapUs);
        ::fflush(stdout);
    }

    void shutdown_service_interface(areg::Component & holder) noexcept final
    {
        mQuit.store(true);
        mTimer.stop_timer();
        mThread.shutdown(areg::WAIT_INFINITE);
        MixedTrafficProviderBase::shutdown_service_interface(holder);
    }

    void request_ping(uint64_t stamp) final
    {
        response_pong(stamp);
    }

    void on_run() final
    {
        const uint32_t bytes{ options().bytes };
        const uint32_t gapUs{ options().gapUs };
        areg::Wait wait;
        std::vector<uint8_t> data(bytes);
        uint32_t seq{ 0u };

        while (mQuit.load() == false)
        {
            ++seq;
            fill_block(data.data(), bytes, seq);
            broadcast_bulk_block(areg::SharedBuffer(data.data(), bytes), seq);
            mSent.store(seq);

            if (gapUs != 0u)
            {
                wait.wait_for(std::chrono::microseconds{ gapUs });
            }
        }
    }

    void process_timer(areg::Timer & /*timer*/) final
    {
        ::printf("P %llu sent=%u\n", static_cast<unsigned long long>(now_ms()), mSent.load());
        ::fflush(stdout);
    }

private:
    areg::Thread            mThread;
    areg::Timer             mTimer;
    std::atomic_uint32_t    mSent;
    std::atomic_bool        mQuit;
};

//////////////////////////////////////////////////////////////////////////
// Attribute provider
//////////////////////////////////////////////////////////////////////////

//!< Sets an always-notified attribute to the next number every ATTRIBUTE_PERIOD_MS.
class AttributeProvider final   : public    areg::Component
                                , protected PubSubProviderBase
                                , protected areg::TimerConsumer
{
public:
    AttributeProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component       ( entry, owner )
        , PubSubProviderBase    ( static_cast<areg::Component &>(*this) )
        , areg::TimerConsumer   ( )
        , mTimer                ( static_cast<areg::TimerConsumer &>(*this), "ConnScenarioAttribute" )
        , mValue                ( 0u )
    {
    }

protected:
    void startup_service_interface(areg::Component & holder) final
    {
        PubSubProviderBase::startup_service_interface(holder);
        mTimer.start_timer(ATTRIBUTE_PERIOD_MS, component_thread(), areg::Timer::CONTINUOUSLY);
    }

    void shutdown_service_interface(areg::Component & holder) noexcept final
    {
        mTimer.stop_timer();
        PubSubProviderBase::shutdown_service_interface(holder);
    }

    void process_timer(areg::Timer & /*timer*/) final
    {
        set_integer_always(++mValue);
    }

private:
    areg::Timer mTimer;
    uint32_t    mValue;
};

BEGIN_MODEL(PROVIDER_MODEL)
    BEGIN_REGISTER_THREAD("ConnScenarioProviderThread")
        BEGIN_REGISTER_COMPONENT(ROLE_PROVIDER, ScenarioProvider)
            REGISTER_IMPLEMENT_SERVICE(MixedTraffic::ServiceName, MixedTraffic::InterfaceVersion)
        END_REGISTER_COMPONENT(ROLE_PROVIDER)
        BEGIN_REGISTER_COMPONENT(ROLE_ATTRIBUTE, AttributeProvider)
            REGISTER_IMPLEMENT_SERVICE(PubSub::ServiceName, PubSub::InterfaceVersion)
        END_REGISTER_COMPONENT(ROLE_ATTRIBUTE)
    END_REGISTER_THREAD("ConnScenarioProviderThread")
END_MODEL(PROVIDER_MODEL)

