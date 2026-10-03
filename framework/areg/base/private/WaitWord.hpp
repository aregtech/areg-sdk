#ifndef AREG_BASE_PRIVATE_WAITWORD_HPP
#define AREG_BASE_PRIVATE_WAITWORD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/WaitWord.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, blocks a thread on a 32-bit word until another thread wakes it:
 *              futex on Linux, __ulock on macOS, WaitOnAddress on Windows and Cygwin. Also
 *              a memory barrier run on every thread of the process.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <atomic>

namespace areg::os
{
    /**
     * \brief   Blocks the calling thread while the word equals the expected value. Returns when
     *          another thread wakes it or spuriously, so the caller re-checks the word.
     *
     * \param   word        The word to wait on.
     * \param   expected    The value the word must hold for the thread to block.
     **/
    void _os_wait_word(const std::atomic<uint32_t> & word, uint32_t expected) noexcept;

    /**
     * \brief   Wakes one thread blocked in _os_wait_word() on the word.
     *
     * \param   word        The word the thread waits on.
     **/
    void _os_wake_word(std::atomic<uint32_t> & word) noexcept;

    /**
     * \brief   Wakes every thread blocked in _os_wait_word() on the word. Reads nothing at the
     *          address, so the word may belong to an object another thread has just released.
     *
     * \param   word        The address of the word the threads wait on.
     **/
    void _os_wake_word_all(const std::atomic<uint32_t> * word) noexcept;

    /**
     * \brief   Returns true if _os_process_barrier() is available in this process. The answer
     *          is decided on the first call and does not change.
     **/
    [[nodiscard]]
    bool _os_has_process_barrier() noexcept;

    /**
     * \brief   Runs a memory barrier on every thread of the process: membarrier on Linux,
     *          FlushProcessWriteBuffers on Windows and Cygwin. Call it only when
     *          _os_has_process_barrier() returns true.
     **/
    void _os_process_barrier() noexcept;
}

#endif  // AREG_BASE_PRIVATE_WAITWORD_HPP
