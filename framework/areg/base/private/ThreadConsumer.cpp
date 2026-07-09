/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/ThreadConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread Consumer class.
 *
 ************************************************************************/
#include "areg/base/ThreadConsumer.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ThreadConsumer class implementation
//////////////////////////////////////////////////////////////////////////

bool ThreadConsumer::on_thread_registered(Thread * /*threadObj*/)
{
    return true;
}

void ThreadConsumer::on_thread_unregistering()
{
}

int32_t ThreadConsumer::on_exit()
{
    return static_cast<int32_t>(ThreadConsumer::ExitCode::Normal);
}

} // namespace areg
