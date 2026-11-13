/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SimpleEventTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the lightweight event of the dispatchers.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/component/private/SimpleEvent.hpp"

TEST(SimpleEventTest, null_event_refuses_every_operation)
{
    areg::SimpleEvent event{ areg::NullTag{} };
    EXPECT_FALSE(event.set_signaled());
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(event.lock(areg::WAIT_INFINITE));
    EXPECT_FALSE(event.reset());
}

TEST(SimpleEventTest, manual_reset_event_created_signaled_stays_signaled_until_reset)
{
    areg::SimpleEvent event(false, false);
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.reset());
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
}

TEST(SimpleEventTest, auto_reset_event_created_signaled_releases_one_wait)
{
    areg::SimpleEvent event(false, true);
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.set_signaled());
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
}
