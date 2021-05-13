/************************************************************************
 * \file        src/services/CESystem.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, tests
 *
 ************************************************************************/
#include "src/services/CESystem.hpp"

#include "areg/base/DateTime.hpp"

Component* CESystem::CreateComponent(const NERegistry::ComponentEntry& entry, ComponentThread& owner)
{
    return DEBUG_NEW CESystem(owner, entry.mRoleName.getString(), entry.getComponentData());
}

void CESystem::DeleteComponent(Component& compObject, const NERegistry::ComponentEntry& entry)
{
    delete (&compObject);
}

CESystem::CESystem(ComponentThread& masterThread, const char* const roleName, NEMemory::uAlign data)
    : Component       ( masterThread, roleName)
    , SystemStub      ( static_cast<Component &>(self()) )
    , IETimerConsumer ( )

    , mTimer            ( static_cast<IETimerConsumer &>(self()), "test_areg_basic_system_timer")
{
}

CESystem::~CESystem( void )
{

}

void CESystem::requestPowerOff(void)
{
}

void CESystem::requestPowerOn(void)
{
}

void CESystem::requestShutdown(void)
{
}

void CESystem::requestStart(void)
{
}

void CESystem::startupServiceInterface(Component& holder)
{
    SystemStub::startupServiceInterface(holder);

    setSystemState(NESystem::SystemPowerOff);
    mTimer.startTimer(100, 20);

    printf("System >>> Start Timer [ %s ] at [ %s ] with timeout [ %d ] ms\n"
            , mTimer.getName().getString()
            , DateTime::getNow().formatTime().getString()
            , mTimer.getTimeoutValue());
}

void CESystem::shutdownComponent(ComponentThread& comThread)
{
    printf("System >>> Timer [ %s ] is stopped at [ %s ]...\n"
            , mTimer.getName().getString()
            , DateTime::getNow().formatTime().getString());
    mTimer.stopTimer();
}

void CESystem::processTimer(Timer& timer)
{
    if (&timer == &mTimer)
    {
        printf("System >>> Timer [ %s ] is processed at [ %s ]... \n"
                , mTimer.getName().getString()
                , DateTime::getNow().formatTime().getString());
    }
}

