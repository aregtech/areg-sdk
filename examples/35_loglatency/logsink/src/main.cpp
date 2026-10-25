//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : Log sink of the logging benchmark. It is a log observer: it
//               receives the messages of 35_logsource through the log
//               collector, counts them and measures the time from the
//               LOG_DBG call in the source until the message arrives here.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/base/DateTime.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areglogger/client/LogObserverApi.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "areglogger")
#endif // _MSC_VER

namespace
{
    //!< Milliseconds without a message after which the run is over.
    constexpr uint32_t  IDLE_MS         { 3'000u };
    //!< Milliseconds the sink waits for the first message.
    constexpr uint32_t  FIRST_MS        { 60'000u };

    //!< The state of a run, written by the receive thread of the observer.
    struct Run
    {
        std::vector<uint32_t>   latencies   { };            //!< Microseconds from the call to the arrival.
        std::atomic<uint32_t>   received    { 0u };         //!< Messages received.
        uint64_t                firstUs     { 0u };         //!< Arrival of the first message.
        uint64_t                lastUs      { 0u };         //!< Arrival of the last message.
        uint32_t                expected    { 0u };         //!< Messages that end the run, 0 for no limit.
        std::mutex              lock        { };            //!< Guards the wake-up of the main thread.
        std::condition_variable changed     { };            //!< Signalled for every message and at the end.
        bool                    connected   { false };      //!< True when the observer is connected to the log collector.
    };

    Run _run;

    void on_log_message(const unsigned char * logBuffer, uint32_t size)
    {
        if ((logBuffer == nullptr) || (size < areg::log_entry_head()))
            return;

        const areg::LogEntry & entry{ *reinterpret_cast<const areg::LogEntry *>(logBuffer) };
        if ((entry.logMsgType != areg::LogMessageType::MessageText) || (entry.logDataType != areg::LogDataType::Remote))
            return;

        const uint64_t now{ static_cast<uint64_t>(areg::DateTime::now()) };
        const uint64_t sent{ static_cast<uint64_t>(entry.logTimestamp) };
        if (_run.latencies.size() < _run.latencies.capacity())
        {
            _run.latencies.push_back(static_cast<uint32_t>(now > sent ? now - sent : 0u));
        }

        if (_run.firstUs == 0u)
        {
            _run.firstUs = now;
        }

        _run.lastUs = now;
        const uint32_t count{ _run.received.fetch_add(1u) + 1u };
        if ((count == 1u) || (count == _run.expected))
        {
            std::lock_guard<std::mutex> guard(_run.lock);
            _run.changed.notify_all();
        }
    }

    void on_connected(bool isConnected, const char * /* address */, uint16_t /* port */)
    {
        std::lock_guard<std::mutex> guard(_run.lock);
        _run.connected = isConnected;
        _run.changed.notify_all();
    }

    double percentile(const std::vector<uint32_t> & sorted, double fraction)
    {
        const std::size_t index{ static_cast<std::size_t>(fraction * static_cast<double>(sorted.size() - 1u)) };
        return static_cast<double>(sorted[index]);
    }
}

int main(int argc, char ** argv)
{
    uint32_t expected{ 0u };
    for (int i = 1; i < argc; ++i)
    {
        if (std::strncmp(argv[i], "-c=", 3) == 0)
        {
            expected = static_cast<uint32_t>(std::strtoul(argv[i] + 3, nullptr, 10));
        }
        else
        {
            std::printf("Usage: 35_logsink [-c=<messages that end the run>]\n");
            return 1;
        }
    }

    _run.expected = expected;
    _run.latencies.reserve(expected != 0u ? expected : 10'000'000u);

    ObserverEvents events{ };
    events.evtLogMessageEx      = &on_log_message;
    events.evtServiceConnected  = &on_connected;
    if (log_observer_initialize(&events, nullptr) == false)
    {
        std::printf("35_logsink: failed to initialize the log observer.\n");
        return 2;
    }

    static_cast<void>(log_observer_connect_logger(nullptr, nullptr, 0u));
    {
        std::unique_lock<std::mutex> guard(_run.lock);
        if (_run.changed.wait_for(guard, std::chrono::milliseconds(FIRST_MS), [] { return _run.connected; }) == false)
        {
            guard.unlock();
            std::printf("35_logsink: the log collector is not reachable, start logcollector first.\n");
            log_observer_release();
            return 2;
        }

        std::printf("35_logsink: connected, waiting for the messages of 35_logsource.\n");
        std::fflush(stdout);
        _run.changed.wait_for(guard, std::chrono::milliseconds(FIRST_MS), [] { return _run.received.load() != 0u; });
        uint32_t seen{ 0u };
        while ((_run.received.load() != seen) && ((expected == 0u) || (_run.received.load() < expected)))
        {
            seen = _run.received.load();
            _run.changed.wait_for(guard, std::chrono::milliseconds(IDLE_MS), [expected] { return (expected != 0u) && (_run.received.load() >= expected); });
        }
    }

    log_observer_disconnect_logger();
    log_observer_release();

    std::vector<uint32_t> & latencies{ _run.latencies };
    const uint32_t received{ _run.received.load() };
    if (latencies.empty())
    {
        std::printf("35_logsink: no message received.\n");
        return 3;
    }

    std::sort(latencies.begin(), latencies.end());
    double sum{ 0.0 };
    for (uint32_t sample : latencies)
    {
        sum += sample;
    }

    const uint64_t spanUs{ _run.lastUs > _run.firstUs ? _run.lastUs - _run.firstUs : 1u };
    std::printf("35 logsink   | received %u | lost %u | rate %.0f msg/s\n"
               , received, (expected > received) ? expected - received : 0u
               , static_cast<double>(received) * 1e6 / static_cast<double>(spanUs));
    std::printf("35 log trip  | P50 %.0f us | P90 %.0f us | P99 %.0f us | P99.9 %.0f us | max %.0f us | mean %.0f us\n"
               , percentile(latencies, 0.50), percentile(latencies, 0.90), percentile(latencies, 0.99)
               , percentile(latencies, 0.999), static_cast<double>(latencies.back()), sum / static_cast<double>(latencies.size()));
    std::fflush(stdout);
    return 0;
}
