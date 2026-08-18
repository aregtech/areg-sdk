/************************************************************************
 * \file        src/LocalLatencyProvider.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Service provider of the local latency benchmark (example 31).
 ************************************************************************/

#include "src/LocalLatencyProvider.hpp"

#include "common/common.hpp"
#include "src/LatencyOptions.hpp"

LocalLatencyProvider::LocalLatencyProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component           (entry, owner)
    , LocalLatencyProviderBase  (static_cast<areg::Component &>(*this))
{
}

void LocalLatencyProvider::request_message_next(LocalLatency::LatencyMode mode, uint32_t id)
{
    // The timestamp must be the last thing taken before the send: everything between the
    // two would be counted as message latency.
    const uint64_t begin{ loclat::now_ns() };

    switch (mode)
    {
    case LocalLatency::LatencyMode::Broadcast0:
        broadcast_message_0(id, begin);
        break;
    case LocalLatency::LatencyMode::Broadcast8:
        broadcast_message_8(id, begin, Latency::latency8());
        break;
    case LocalLatency::LatencyMode::Broadcast16:
        broadcast_message_16(id, begin, Latency::latency16());
        break;
    case LocalLatency::LatencyMode::Broadcast32:
        broadcast_message_32(id, begin, Latency::latency32());
        break;
    case LocalLatency::LatencyMode::Broadcast64:
        broadcast_message_64(id, begin, Latency::latency64());
        break;
    case LocalLatency::LatencyMode::Broadcast128:
        broadcast_message_128(id, begin, Latency::latency128());
        break;
    case LocalLatency::LatencyMode::Broadcast256:
        broadcast_message_256(id, begin, Latency::latency256());
        break;
    case LocalLatency::LatencyMode::Broadcast512:
        broadcast_message_512(id, begin, Latency::latency512());
        break;
    case LocalLatency::LatencyMode::Broadcast1024:
        broadcast_message_1024(id, begin, Latency::latency1024());
        break;
    case LocalLatency::LatencyMode::Broadcast4096:
        broadcast_message_4096(id, begin, Latency::latency4096());
        break;
    case LocalLatency::LatencyMode::Broadcast65536:
        broadcast_message_65536(id, begin, Latency::latency65536());
        break;
    default:
        break;
    }
}

void LocalLatencyProvider::request_ping_pong_0(uint32_t id, uint64_t begin)
{
    response_ping_pong_0(id, begin, loclat::now_ns());
}

void LocalLatencyProvider::request_ping_pong_8(uint32_t id, uint64_t begin, const LocalLatency::Data8 & data8)
{
    response_ping_pong_8(id, begin, loclat::now_ns(), data8);
}

void LocalLatencyProvider::request_ping_pong_16(uint32_t id, uint64_t begin, const LocalLatency::Data16 & data16)
{
    response_ping_pong_16(id, begin, loclat::now_ns(), data16);
}

void LocalLatencyProvider::request_ping_pong_32(uint32_t id, uint64_t begin, const LocalLatency::Data32 & data32)
{
    response_ping_pong_32(id, begin, loclat::now_ns(), data32);
}

void LocalLatencyProvider::request_ping_pong_64(uint32_t id, uint64_t begin, const LocalLatency::Data64 & data64)
{
    response_ping_pong_64(id, begin, loclat::now_ns(), data64);
}

void LocalLatencyProvider::request_ping_pong_128(uint32_t id, uint64_t begin, const LocalLatency::Data128 & data128)
{
    response_ping_pong_128(id, begin, loclat::now_ns(), data128);
}

void LocalLatencyProvider::request_ping_pong_256(uint32_t id, uint64_t begin, const LocalLatency::Data256 & data256)
{
    response_ping_pong_256(id, begin, loclat::now_ns(), data256);
}

void LocalLatencyProvider::request_ping_pong_512(uint32_t id, uint64_t begin, const LocalLatency::Data512 & data512)
{
    response_ping_pong_512(id, begin, loclat::now_ns(), data512);
}

void LocalLatencyProvider::request_ping_pong_1024(uint32_t id, uint64_t begin, const LocalLatency::Data1024 & data1024)
{
    response_ping_pong_1024(id, begin, loclat::now_ns(), data1024);
}

void LocalLatencyProvider::request_ping_pong_4096(uint32_t id, uint64_t begin, const LocalLatency::Data4096 & data4096)
{
    response_ping_pong_4096(id, begin, loclat::now_ns(), data4096);
}

void LocalLatencyProvider::request_ping_pong_65536(uint32_t id, uint64_t begin, const LocalLatency::Data65536 & data65536)
{
    response_ping_pong_65536(id, begin, loclat::now_ns(), data65536);
}
