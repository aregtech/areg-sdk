/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServiceApplicationBasePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service application base class.
 *              POSIX specific implementation.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/ServiceApplicationBase.hpp"

#ifdef _POSIX

//////////////////////////////////////////////////////////////////////////
// ServiceApplicationBase class POSIX specific implementation
//////////////////////////////////////////////////////////////////////////
bool aregext::ServiceApplicationBase::_osIsValid() const
{
    return true;
}

bool aregext::ServiceApplicationBase::_osRegisterService()
{
    return true;
}

void aregext::ServiceApplicationBase::_osFreeResources()
{
    mSvcHandle = nullptr;
    mSeMHandle = nullptr;
}

bool aregext::ServiceApplicationBase::_osInitializeService()
{
    return true;
}

bool aregext::ServiceApplicationBase::_osOpenService()
{
    return true;
}

bool aregext::ServiceApplicationBase::_osCreateService()
{
    return true;
}

void aregext::ServiceApplicationBase::_osDeleteService()
{
}

bool aregext::ServiceApplicationBase::_osSetState(aregext::ServicePhase newState)
{
    bool result{ true };

    if (newState != mSystemServiceState)
    {
        switch (newState)
        {
        case aregext::ServicePhase::Stopped:
            break;

        case aregext::ServicePhase::Starting:
            break;

        case aregext::ServicePhase::Stopping:
            break;

        case aregext::ServicePhase::Running:
            break;

        case aregext::ServicePhase::Continuing:
            break;

        case aregext::ServicePhase::Pausing:
            break;

        case aregext::ServicePhase::Paused:
            break;

        default:
            ASSERT(false);
        }

        mSystemServiceState = newState;
    }

    return result;
}

int32_t aregext::ServiceApplicationBase::_osStartServiceDispatcher()
{
    return RESULT_IGNORED;
}

#endif  // _POSIX
