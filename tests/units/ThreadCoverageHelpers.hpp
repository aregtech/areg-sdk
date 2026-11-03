#ifndef AREG_TESTS_UNITS_THREADCOVERAGEHELPERS_HPP
#define AREG_TESTS_UNITS_THREADCOVERAGEHELPERS_HPP

#include "units/GUnitTest.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <thread>

namespace
{
    constexpr uint32_t COVERAGE_WAIT_MS{ 30'000u };
    constexpr uint32_t COVERAGE_NEVER_MS{ 3'600'000u };

    class CoverageGate
    {
    public:
        CoverageGate()
            : mMutex()
            , mChanged()
            , mCount(0u)
        {
        }

        void arrive(uint32_t count = 1u)
        {
            {
                std::lock_guard<std::mutex> lock(mMutex);
                mCount += count;
            }

            mChanged.notify_all();
        }

        bool wait(uint32_t count = 1u)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            return mChanged.wait_for(lock, std::chrono::milliseconds(COVERAGE_WAIT_MS)
                , [this, count]() { return mCount >= count; });
        }

    private:
        std::mutex mMutex;
        std::condition_variable mChanged;
        uint32_t mCount;
    };

    class CoverageDeadline
    {
    public:
        CoverageDeadline()
            : mMutex()
            , mChanged()
            , mFinished(false)
            , mWatchdog([this]()
                {
                    std::unique_lock<std::mutex> lock(mMutex);
                    if (!mChanged.wait_for(lock, std::chrono::seconds(60), [this]() { return mFinished; }))
                    {
                        ADD_FAILURE() << "Threaded test exceeded its deadlock deadline";
                        std::abort();
                    }
                })
        {
        }

        ~CoverageDeadline()
        {
            {
                std::lock_guard<std::mutex> lock(mMutex);
                mFinished = true;
            }

            mChanged.notify_all();
            mWatchdog.join();
        }

    private:
        std::mutex mMutex;
        std::condition_variable mChanged;
        bool mFinished;
        std::thread mWatchdog;
    };
}

#endif // AREG_TESTS_UNITS_THREADCOVERAGEHELPERS_HPP
