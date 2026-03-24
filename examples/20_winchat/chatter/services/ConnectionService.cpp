/************************************************************************
 * \file            chatter/services/ConnectionService.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "chatter/res/stdafx.h"
#include "areg/base/areg_global.h"
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

ConnectionService::ConnectionService( const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread )
    : areg::Component       ( entry, ownerThread )
{
}

void ConnectionService::startup_component( areg::ComponentThread & comThread )
{
    areg::Component::startup_component(comThread);
    DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceStartup, 1, reinterpret_cast<LPARAM>(this) );
}

void ConnectionService::shutdown_component( areg::ComponentThread & comThread ) noexcept
{
    DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceStartup, 0, 0 );
    areg::Component::shutdown_component(comThread);
}
