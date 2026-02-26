/************************************************************************
 * \file            chatter/services/ConnectionService.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "chatter/res/stdafx.h"
#include "areg/base/GEGlobal.h"
#include "chatter/services/ConnectionService.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "common/ChatDefs.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/DistrbutedApp.hpp"
#include "chatter/ui/DistributedDialog.hpp"

BEGIN_MODEL(chat::MODEL_NAME_DISTRIBUTED_CLIENT)

    BEGIN_REGISTER_THREAD( NEDistributedApp::THREAD_DISTRIBUTED )
        BEGIN_REGISTER_COMPONENT( chat::COMP_NAME_DISTRIBUTED_CLIENT, ConnectionService )
            REGISTER_DEPENDENCY( chat::COMP_NAME_CENTRAL_SERVER )
        END_REGISTER_COMPONENT( chat::COMP_NAME_DISTRIBUTED_CLIENT )
    END_REGISTER_THREAD( NEDistributedApp::THREAD_DISTRIBUTED )

END_MODEL(chat::MODEL_NAME_DISTRIBUTED_CLIENT)

DEF_LOG_SCOPE( chatter_ConnectionService_CreateComponent );
DEF_LOG_SCOPE( chatter_ConnectionService_DeleteComponent );

ConnectionService::ConnectionService( const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread )
    : Component       ( entry, ownerThread )
{
}

void ConnectionService::startupComponent( areg::ComponentThread & comThread )
{
    Component::startupComponent(comThread);
    DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceStartup, 1, reinterpret_cast<LPARAM>(this) );
}

void ConnectionService::shutdownComponent( areg::ComponentThread & comThread )
{
    DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceStartup, 0, 0 );
    Component::shutdownComponent(comThread);
}
