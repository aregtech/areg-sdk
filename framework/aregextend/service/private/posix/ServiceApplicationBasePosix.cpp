/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
namespace areg::ext {

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

bool ServiceApplicationBase::_os_set_state(areg::ext::ServicePhase newState)
{
    bool result{ true };

    if (newState != mSystemServiceState)
    {
        switch (newState)
        {
        case areg::ext::ServicePhase::Stopped:
            break;

        case areg::ext::ServicePhase::Starting:
            break;

        case areg::ext::ServicePhase::Stopping:
            break;

        case areg::ext::ServicePhase::Running:
            break;

        case areg::ext::ServicePhase::Continuing:
            break;

        case areg::ext::ServicePhase::Pausing:
            break;

        case areg::ext::ServicePhase::Paused:
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

} // namespace areg::ext

#endif  // _POSIX
