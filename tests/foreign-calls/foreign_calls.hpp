#ifndef AREG_TESTS_FOREIGN_CALLS_FOREIGN_CALLS_HPP
#define AREG_TESTS_FOREIGN_CALLS_FOREIGN_CALLS_HPP
/**
 * \file    foreign_calls.hpp
 * \brief   Shared state of the foreign calls test: the calls a foreign thread makes on the
 *          provider, and the counters both models update.
 */

#include "areg/base/areg_global.h"

#include <atomic>

namespace foreign
{
    //!< The provider calls a foreign thread makes.
    class ProviderCalls
    {
    public:
        virtual ~ProviderCalls() = default;
        virtual void call_set_reading(int16_t value) = 0;
        virtual void call_broadcast_alarm_raised(int16_t value) = 0;
        virtual void call_response_set_thresholds() = 0;
    };

    //!< The provider while its service interface is up, otherwise null.
    extern std::atomic<ProviderCalls *> gProvider;

    //!< Requests of set_thresholds the provider has left pending.
    extern std::atomic_uint32_t gPendingThresholds;
}

#endif // AREG_TESTS_FOREIGN_CALLS_FOREIGN_CALLS_HPP
