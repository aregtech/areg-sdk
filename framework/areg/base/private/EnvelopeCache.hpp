#ifndef AREG_BASE_PRIVATE_ENVELOPECACHE_HPP
#define AREG_BASE_PRIVATE_ENVELOPECACHE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/EnvelopeCache.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, a bounded cache of freed message envelope blocks.
 *              Every block is a plain `new uint8_t[]` block of the event header plus the
 *              payload. Each thread keeps two magazines of blocks per size class and
 *              exchanges full and empty magazines with a capped process-wide depot.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

namespace areg
{
    /**
     * \brief   Allocates an envelope block of the event header plus at least 'payload' bytes,
     *          from the calling thread's cache when it holds one.
     *
     * \param   payload     On input, the payload bytes needed. On output, the payload bytes
     *                      the block holds: the size class, or the input above the largest class.
     * \return  The block, or nullptr if there is no memory.
     **/
    [[nodiscard]]
    uint8_t * envelope_block_alloc(uint32_t & payload) noexcept;

    /**
     * \brief   Returns the calling thread's cached blocks and the depot's blocks to the heap.
     **/
    AREG_API void envelope_cache_trim() noexcept;

    /**
     * \brief   Returns the number of blocks cached for the size class of 'payload' bytes:
     *          the calling thread's magazines plus the depot. 0 if the size has no class.
     **/
    [[nodiscard]]
    AREG_API uint32_t envelope_cache_held(uint32_t payload) noexcept;
}

#endif  // AREG_BASE_PRIVATE_ENVELOPECACHE_HPP
