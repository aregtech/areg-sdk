/************************************************************************
 * \file        src/LocalLatencyProvider.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Service provider of the local latency benchmark (example 31).
 ************************************************************************/

#ifndef EXAMPLES_31_LOCLATENCY_SRC_LOCALLATENCYPROVIDER_HPP
#define EXAMPLES_31_LOCLATENCY_SRC_LOCALLATENCYPROVIDER_HPP

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/31_loclatency/services/LocalLatencyProviderBase.hpp"

/**
 * \brief   The service side of the local latency benchmark.
 *
 *          The provider does as little as possible, on purpose. Everything it does adds
 *          to the measured time, so any extra work would hide what the benchmark wants
 *          to see: the cost the framework needs to move one message.
 *
 *          It answers exactly two kinds of call:
 *
 *          1. `request_message_next` -- the one-way trip. The consumer asks for one
 *             message; the provider takes a timestamp and sends one broadcast of the
 *             requested size. It never sends on its own, so at most one message is on
 *             the way at any moment and the test can never run away.
 *
 *          2. `request_ping_pong_*` -- the round trip. The provider takes a timestamp
 *             and answers straight away, giving back the consumer's own send timestamp
 *             together with its own. The consumer can then split the round trip into
 *             the way there and the way back.
 *
 *          The provider keeps no test state at all. Which size to send is a parameter of
 *          every single call, so provider and consumer can never fall out of step.
 **/
class LocalLatencyProvider final : public    areg::Component
                                 , protected LocalLatencyProviderBase
{
    friend class areg::Component;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by the framework when the component thread starts.
     *
     * \param   entry   The registry entry that describes this component.
     * \param   owner   The component thread that owns this component.
     **/
    LocalLatencyProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

    ~LocalLatencyProvider() = default;

//////////////////////////////////////////////////////////////////////////
// LocalLatencyProviderBase overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sends exactly one broadcast of the requested size.
     *
     *          The timestamp that the consumer will measure against is taken here, right
     *          before the broadcast call, so the measured time contains only the way from
     *          the provider to the consumer. The pull call that led here is not measured.
     *
     * \param   mode    Which broadcast to send. Only the Broadcast* values do something;
     *                  any other value is ignored on purpose, so that a late or wrong
     *                  call cannot spoil a running measurement.
     * \param   id      The sequence number the consumer expects. It is sent back
     *                  unchanged, so the consumer can see whether a message was lost.
     **/
    void request_message_next(LocalLatency::LatencyMode mode, uint32_t id) final;

    /**
     * \brief   Answers a round trip request. See the class description.
     *
     * \param   id      The sequence number given by the consumer; sent back unchanged.
     * \param   begin   The consumer's send timestamp; sent back unchanged.
     **/
    void request_ping_pong_0(uint32_t id, uint64_t begin) final;

    /**
     * \brief   Answers a round trip request that carries 8 bytes of extra data.
     *          The data is sent back unchanged, so the answer is as large as the request.
     *
     * \param   id      The sequence number given by the consumer; sent back unchanged.
     * \param   begin   The consumer's send timestamp; sent back unchanged.
     * \param   data8   The extra payload; sent back unchanged.
     **/
    void request_ping_pong_8(uint32_t id, uint64_t begin, const LocalLatency::Data8 & data8) final;

    /**
     * \brief   Answers a round trip request that carries 16 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_16(uint32_t id, uint64_t begin, const LocalLatency::Data16 & data16) final;

    /**
     * \brief   Answers a round trip request that carries 32 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_32(uint32_t id, uint64_t begin, const LocalLatency::Data32 & data32) final;

    /**
     * \brief   Answers a round trip request that carries 64 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_64(uint32_t id, uint64_t begin, const LocalLatency::Data64 & data64) final;

    /**
     * \brief   Answers a round trip request that carries 128 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_128(uint32_t id, uint64_t begin, const LocalLatency::Data128 & data128) final;

    /**
     * \brief   Answers a round trip request that carries 256 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_256(uint32_t id, uint64_t begin, const LocalLatency::Data256 & data256) final;

    /**
     * \brief   Answers a round trip request that carries 512 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_512(uint32_t id, uint64_t begin, const LocalLatency::Data512 & data512) final;

    /**
     * \brief   Answers a round trip request that carries 1024 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_1024(uint32_t id, uint64_t begin, const LocalLatency::Data1024 & data1024) final;

    /**
     * \brief   Answers a round trip request that carries 4096 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_4096(uint32_t id, uint64_t begin, const LocalLatency::Data4096 & data4096) final;

    /**
     * \brief   Answers a round trip request that carries 65536 bytes of extra data.
     * \see     request_ping_pong_8
     **/
    void request_ping_pong_65536(uint32_t id, uint64_t begin, const LocalLatency::Data65536 & data65536) final;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LocalLatencyProvider);
};

#endif  // EXAMPLES_31_LOCLATENCY_SRC_LOCALLATENCYPROVIDER_HPP
