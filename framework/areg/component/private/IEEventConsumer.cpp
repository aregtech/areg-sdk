/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/IEEventConsumer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Consumer base class implementation
 *
 ************************************************************************/
#include "areg/component/IEEventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// IEEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void IEEventConsumer::consumerRegistered( bool /* isRegistered */ )
{
}

bool IEEventConsumer::preprocessEvent( Event & /* eventElem */ )
{
    return true;
}
