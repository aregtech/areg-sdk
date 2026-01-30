/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/IEWorkerThreadConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Worker Thread Consumer.
 *
 ************************************************************************/
#include "areg/component/IEWorkerThreadConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// IEWorkerThreadConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
IEWorkerThreadConsumer::IEWorkerThreadConsumer(const String & consumerName)
    : mConsumerName (consumerName)
{
}
