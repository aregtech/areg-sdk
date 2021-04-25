/************************************************************************
 * \file        src/services/CESystem.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, tests
 *
 ************************************************************************/
#include "src/services/CESystem.hpp"

#include "areg/src/base/CEDateTime.hpp"

CEComponent* CESystem::CreateComponent(const NERegistry::CEComponentEntry& entry, CEComponentThread& owner)
{
    return DEBUG_NEW CESystem(owner, entry.mRoleName.GetBuffer(), entry.GetComponentData());
}

void CESystem::DeleteComponent(CEComponent& compObject, const NERegistry::CEComponentEntry& entry)
{
    delete (&compObject);
}

CESystem::CESystem(CEComponentThread& masterThread, const char* const roleName, NEMemory::uAlign data)
    : CEComponent       (masterThread, roleName)
    , CESystemStub      ( static_cast<CEComponent &>(self()) )
    , IETimerConsumer   ( )

    , mTimer            ( static_cast<IETimerConsumer &>(self()), "test_areg_basic_system_timer")
{
}

CESystem::~CESystem( void )
{

}

void CESystem::RequestPowerOff(void)
{
}

void CESystem::RequestPowerOn(void)
{
}

void CESystem::RequestShutdown(void)
{
}

void CESystem::RequestStart(void)
{
}

void CESystem::StartupServiceInterface(CEComponent& holder)
{
    CESystemStub::StartupServiceInterface(holder);

    SetSystemState(NESystem::SystemPowerOff);
    mTimer.StartTimer(100, 20);

    printf("System >>> Start Timer [ %s ] at [ %s ] with timeout [ %d ] ms\n"
            , mTimer.GetTimerName().GetBuffer()
            , CEDateTime::GetNow().FormatTime().GetBuffer()
            , mTimer.GetTimeoutValue());
}

void CESystem::ShutdownComponent(CEComponentThread& comThread)
{
    printf("System >>> Timer [ %s ] is stopped at [ %s ]...\n"
            , mTimer.GetTimerName().GetBuffer()
            , CEDateTime::GetNow().FormatTime().GetBuffer());
    mTimer.StopTimer();
}

void CESystem::ProcessTimer(CETimer& timer)
{
    if (&timer == &mTimer)
    {
        printf("System >>> Timer [ %s ] is processed at [ %s ]... \n"
                , mTimer.GetTimerName().GetBuffer()
                , CEDateTime::GetNow().FormatTime().GetBuffer());
    }
}

