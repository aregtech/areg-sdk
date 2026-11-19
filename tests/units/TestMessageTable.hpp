#ifndef AREG_TESTS_UNITS_TESTMESSAGETABLE_HPP
#define AREG_TESTS_UNITS_TESTMESSAGETABLE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/TestMessageTable.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The message ID table of a hand-written test interface, built at run time.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceDefs.hpp"

namespace areg_test
{
    /**
     * \brief   A perfect-hash table of a few message IDs of one kind: 256 slots and the first
     *          factor that gives every ID a slot of its own. The ID list must outlive the table.
     **/
    class TestMessageTable
    {
    public:
        static constexpr uint32_t   SLOTS       { 256u };   //!< The number of slots.
        static constexpr uint32_t   SHIFT       { 24u };    //!< The shift of the hash for 256 slots.
        static constexpr uint32_t   MAX_TRIES   { 1u << 20 };   //!< The number of factors tried.

        //!< Builds the table of the IDs. Unique IDs only.
        TestMessageTable(const uint32_t * ids, uint32_t count) noexcept
            : mIds  (ids)
            , mCount(count)
        {
            uint32_t factor{ 0x9E3779B1u };
            for (uint32_t i = 0u; (mCount != 0u) && (i < MAX_TRIES) && !try_factor(factor); ++i)
            {
                factor += 2u;
            }
        }

        //!< Returns the table; an empty one if there is no ID.
        areg::MessageTable table() const noexcept
        {
            return (mCount == 0u ? areg::MessageTable{ } : areg::MessageTable{ mIds, mSlots, mCount, mFactor, SHIFT });
        }

    private:
        //!< Fills the slots if the factor maps every ID to its own slot.
        bool try_factor(uint32_t factor) noexcept
        {
            bool used[SLOTS]{ };
            for (uint32_t i = 0u; i < mCount; ++i)
            {
                const uint32_t slot{ static_cast<uint32_t>(mIds[i] * factor) >> SHIFT };
                if (used[slot])
                    return false;

                used[slot] = true;
            }

            for (uint32_t i = 0u; i < SLOTS; ++i)
            {
                mSlots[i] = 0u;
            }

            for (uint32_t i = 0u; i < mCount; ++i)
            {
                mSlots[static_cast<uint32_t>(mIds[i] * factor) >> SHIFT] = static_cast<uint16_t>(i);
            }

            mFactor = factor;
            return true;
        }

        const uint32_t *    mIds;               //!< The IDs, by index.
        uint32_t            mCount;             //!< The number of IDs.
        uint32_t            mFactor{ 0u };      //!< The factor found.
        uint16_t            mSlots[SLOTS]{ };   //!< The index of every slot.
    };

    /**
     * \brief   Returns the interface data of a hand-written test interface.
     **/
    inline areg::InterfaceData make_interface( const char * name
                                             , const areg::Version & version
                                             , areg::ServiceType type
                                             , const areg::MessageTable & requests
                                             , const areg::MessageTable & responses
                                             , const areg::MessageTable & broadcasts
                                             , const areg::MessageTable & attributes
                                             , const uint16_t * requestToResponse
                                             , const uint32_t * paramCounts ) noexcept
    {
        return areg::InterfaceData{ name, version, type, { requests, responses, broadcasts, attributes }, requestToResponse, paramCounts };
    }
}

#endif  // AREG_TESTS_UNITS_TESTMESSAGETABLE_HPP
