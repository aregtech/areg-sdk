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
bool ServiceApplicationBase::_os_is_valid() const
{
    return true;
}

bool ServiceApplicationBase::_os_register_service()
{
    return true;
}

void ServiceApplicationBase::_os_free_resources()
{
    mSvcHandle = nullptr;
    mSeMHandle = nullptr;
}

bool ServiceApplicationBase::_os_initialize_service()
{
    return true;
}

bool ServiceApplicationBase::_os_open_service()
{
    return true;
}

bool ServiceApplicationBase::_os_create_service()
{
    return true;
}

void ServiceApplicationBase::_os_delete_service()
{
}

bool ServiceApplicationBase::_os_set_state(NESystemService::ServicePhase newState)
{
    bool result{ true };

    if (newState != mSystemServiceState)
    {
        switch (newState)
        {
        case NESystemService::ServicePhase::Stopped:
            break;

        case NESystemService::ServicePhase::Starting:
            break;

        case NESystemService::ServicePhase::Stopping:
            break;

        case NESystemService::ServicePhase::Running:
            break;

        case NESystemService::ServicePhase::Continuing:
            break;

        case NESystemService::ServicePhase::Pausing:
            break;

        case NESystemService::ServicePhase::Paused:
            break;

        default:
            ASSERT(false);
        }

        mSystemServiceState = newState;
    }

    return result;
}

int32_t ServiceApplicationBase::_os_start_service_dispatcher()
{
    return RESULT_IGNORED;
}

#endif  // _POSIX
