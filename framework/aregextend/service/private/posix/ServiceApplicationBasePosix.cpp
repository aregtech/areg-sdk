/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServiceApplicationBasePosix.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service application base class.
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
bool ServiceApplicationBase::_osIsValid(void) const
{
    return true;
}

bool ServiceApplicationBase::_osRegisterService(void)
{
    return true;
}

void ServiceApplicationBase::_osFreeResources(void)
{
    mSvcHandle = nullptr;
    mSeMHandle = nullptr;
}

bool ServiceApplicationBase::_osInitializeService(void)
{
    return true;
}

bool ServiceApplicationBase::_osOpenService(void)
{
    return true;
}

bool ServiceApplicationBase::_osCreateService(void)
{
    return true;
}

void ServiceApplicationBase::_osDeleteService(void)
{
}

bool ServiceApplicationBase::_osSetState(NESystemService::eSystemServiceState newState)
{
    bool result{ true };

    if (newState != mSystemServiceState)
    {
        switch (newState)
        {
        case NESystemService::eSystemServiceState::ServiceStopped:
            break;

        case NESystemService::eSystemServiceState::ServiceStarting:
            break;

        case NESystemService::eSystemServiceState::ServiceStopping:
            break;

        case NESystemService::eSystemServiceState::ServiceRunning:
            break;

        case NESystemService::eSystemServiceState::ServiceContinuing:
            break;

        case NESystemService::eSystemServiceState::ServicePausing:
            break;

        case NESystemService::eSystemServiceState::ServicePaused:
            break;

        default:
            ASSERT(false);
        }

        mSystemServiceState = newState;
    }

    return result;
}

bool ServiceApplicationBase::_osWaitUserInput(char* buffer, unsigned int bufSize)
{
#if __STDC_WANT_LIB_EXT1__
    return(gets_s(buffer, bufSize) != nullptr);
#else   // __STDC_WANT_LIB_EXT1__
    return (fgets(buffer, bufSize, stdin) != nullptr);
#endif  // __STDC_WANT_LIB_EXT1__
}

#endif  // _POSIX
