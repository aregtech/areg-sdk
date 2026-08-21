#ifndef AREG_EXAMPLES_32_PUBMIXED_PUBSERVICE_PROVIDERCOMPONENT_HPP
#define AREG_EXAMPLES_32_PUBMIXED_PUBSERVICE_PROVIDERCOMPONENT_HPP
/************************************************************************
 * \file        pubservice/src/ProviderComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Example 32: provider of the mixed traffic service.
 *
 *              Several worker threads broadcast large blocks on one connection while the same
 *              connection answers small ping requests. That is the case none of the other
 *              examples cover: example 30 and 31 send only small messages, so their queue never
 *              fills, and example 23 sends only large ones, so no small message ever has to wait
 *              behind a big one.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "examples/32_pubmixed/services/MixedTrafficProviderBase.hpp"
#include "common/MixedDefs.hpp"

#include <atomic>
#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// ProviderComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Answers ping requests, and runs a configurable number of bulk producer threads that
 *          all broadcast on the same connection.
 **/
class ProviderComponent final   : public    areg::Component
                                , protected MixedTrafficProviderBase
                                , protected areg::ThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    //!< Receives the report timer.
    class ReportTimerConsumer : public areg::TimerConsumer
    {
    public:
        explicit ReportTimerConsumer(ProviderComponent& owner)
            : areg::TimerConsumer   ( )
            , mOwner                ( owner )
        {
        }

        virtual ~ReportTimerConsumer() = default;

    private:
        void process_timer(areg::Timer& timer) final;

        ProviderComponent&  mOwner;

        ReportTimerConsumer() = delete;
        AREG_NOCOPY_NOMOVE(ReportTimerConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr std::string_view   TIMER_NAME      { "MixedReportTimer" };
    static constexpr std::string_view   THREAD_PREFIX   { "MixedBulkThread" };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ProviderComponent(const areg::ComponentEntry& entry, areg::ComponentThread& owner);

    virtual ~ProviderComponent() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
    void startup_service_interface(areg::Component& holder) final;

    void shutdown_service_interface(areg::Component& holder) noexcept final;

    /**
     * \brief   Answers the ping immediately, on the thread the request arrived on.
     **/
    void request_ping(uint64_t stamp) final;

    /**
     * \brief   Body of every bulk producer thread.
     **/
    void on_run() final;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ProviderComponent& self();

    void on_report_timer();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< One pre-built block, shared by every producer: the example measures the pipeline, not
    //!< the cost of filling a buffer.
    areg::SharedBuffer                          mBlock;
    //!< The bulk producer threads.
    std::vector<std::unique_ptr<areg::Thread>>  mProducers;
    //!< Blocks broadcast since the last report.
    std::atomic_uint32_t                        mBulkBlocks;
    //!< Pings answered since the last report.
    std::atomic_uint32_t                        mPings;
    //!< Tells the producer threads to stop.
    std::atomic_bool                            mQuit;
    //!< The report timer.
    areg::Timer                                 mTimer;
    //!< Receives the report timer.
    ReportTimerConsumer                         mTimerConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ProviderComponent() = delete;
    AREG_NOCOPY_NOMOVE(ProviderComponent);
};

//////////////////////////////////////////////////////////////////////////
// ProviderComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ProviderComponent& ProviderComponent::self()
{
    return (*this);
}

#endif // AREG_EXAMPLES_32_PUBMIXED_PUBSERVICE_PROVIDERCOMPONENT_HPP
