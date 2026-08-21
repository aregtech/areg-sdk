#ifndef AREG_EXAMPLES_32_PUBMIXED_PUBCLIENT_CONSUMERCOMPONENT_HPP
#define AREG_EXAMPLES_32_PUBMIXED_PUBCLIENT_CONSUMERCOMPONENT_HPP
/************************************************************************
 * \file        pubclient/src/ConsumerComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Example 32: consumer of the mixed traffic service.
 *
 *              Receives the bulk stream and, on the same connection, sends small pings and
 *              measures their round-trip time. The round-trip time while the bulk stream runs,
 *              compared with the same figure when the provider is started with `-q`, is the
 *              number this example exists to produce.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "examples/32_pubmixed/services/MixedTrafficConsumerBase.hpp"
#include "common/MixedDefs.hpp"

#include <cstdint>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// ConsumerComponent class declaration
//////////////////////////////////////////////////////////////////////////
class ConsumerComponent final   : public    areg::Component
                                , protected MixedTrafficConsumerBase
                                , protected areg::TimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr std::string_view   TIMER_PING      { "MixedPingTimer" };
    static constexpr std::string_view   TIMER_REPORT    { "MixedReportTimer" };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ConsumerComponent(const areg::ComponentEntry& entry, areg::ComponentThread& owner);

    virtual ~ConsumerComponent() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase& proxy) override;

    /**
     * \brief   Counts the bulk block. The payload is not touched: this example measures the
     *          pipeline, not what an application would do with the data.
     **/
    void broadcast_bulk_block(const areg::SharedBuffer& block, uint32_t producer) override;

    /**
     * \brief   Closes one round trip and records it.
     **/
    void response_pong(uint64_t stamp) override;

    void broadcast_service_stopping() override;

    void process_timer(areg::Timer& timer) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ConsumerComponent& self();

    void send_ping();

    void report();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    areg::Timer             mPingTimer;     //!< Drives the ping rate.
    areg::Timer             mReportTimer;   //!< Drives the console line.

    std::vector<uint32_t>   mRtt;           //!< Round-trip samples of the current second, microseconds.
    std::vector<uint32_t>   mRttAll;        //!< Every round-trip sample of the run.

    uint64_t                mBulkBytes;     //!< Bulk bytes since the last report.
    uint32_t                mBulkBlocks;    //!< Bulk blocks since the last report.
    uint32_t                mPingsSent;     //!< Pings sent but not yet answered plus answered.
    uint32_t                mPingsLost;     //!< Pings that were never answered.
    uint32_t                mSeconds;       //!< Reports printed so far.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ConsumerComponent() = delete;
    AREG_NOCOPY_NOMOVE(ConsumerComponent);
};

//////////////////////////////////////////////////////////////////////////
// ConsumerComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ConsumerComponent& ConsumerComponent::self()
{
    return (*this);
}

#endif // AREG_EXAMPLES_32_PUBMIXED_PUBCLIENT_CONSUMERCOMPONENT_HPP
