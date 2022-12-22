/************************************************************************
 * \file            chatter/services/ConnectionService.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "chatter/res/stdafx.h"
#include "areg/base/GEGlobal.h"
#include "chatter/services/ConnectionService.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "generated/NECommon.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/DistrbutedApp.hpp"
#include "chatter/ui/DistributedDialog.hpp"

BEGIN_MODEL(NECommon::MODEL_NAME_DISTRIBUTED_CLIENT)

    BEGIN_REGISTER_THREAD( NEDistributedApp::THREAD_DISTRIBUTED, NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT( NECommon::COMP_NAME_DISTRIBUTED_CLIENT, ConnectionService )
            REGISTER_DEPENDENCY( NECommon::COMP_NAME_CENTRAL_SERVER )
        END_REGISTER_COMPONENT( NECommon::COMP_NAME_DISTRIBUTED_CLIENT )
    END_REGISTER_THREAD( NEDistributedApp::THREAD_DISTRIBUTED )

END_MODEL(NECommon::MODEL_NAME_DISTRIBUTED_CLIENT)

DEF_TRACE_SCOPE( distrbutedapp_ConnectionService_CreateComponent );
DEF_TRACE_SCOPE( distrbutedapp_ConnectionService_DeleteComponent );

Component * ConnectionService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    TRACE_SCOPE( distrbutedapp_ConnectionService_CreateComponent );
    return new ConnectionService( entry, owner );
}

void ConnectionService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    TRACE_SCOPE( distrbutedapp_ConnectionService_DeleteComponent );
    delete (&compObject);
}

ConnectionService::ConnectionService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread )
    : Component       ( entry, ownerThread )
{
}

void ConnectionService::startupComponent( ComponentThread & comThread )
{
    Component::startupComponent(comThread);
    DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdServiceStartup, 1, reinterpret_cast<LPARAM>(this) );
}

void ConnectionService::shutdownComponent( ComponentThread & comThread )
{
    DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdServiceStartup, 0, 0 );
    Component::shutdownComponent(comThread);
}
