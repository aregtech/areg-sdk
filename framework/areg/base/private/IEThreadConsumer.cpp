/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/IEThreadConsumer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread Consumer class.
 *
 ************************************************************************/
#include "areg/base/IEThreadConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// IEThreadConsumer class implementation
//////////////////////////////////////////////////////////////////////////

bool IEThreadConsumer::onThreadRegistered(Thread * /*threadObj*/)
{
    return true;
}

void IEThreadConsumer::onThreadUnregistering(void)
{

}

int IEThreadConsumer::onThreadExit(void)
{
    return static_cast<int>(IEThreadConsumer::eExitCodes::ExitNormal);
}
