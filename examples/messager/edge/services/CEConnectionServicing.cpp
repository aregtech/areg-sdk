/************************************************************************
 * \file            edge/services/CEConnectionServicing.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "edge/res/stdafx.h"
#include "areg/base/GEGlobal.h"
#include "edge/services/CEConnectionServicing.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/CEComponentLoader.hpp"
#include "areg/component/CEComponentThread.hpp"
#include "shared/NECommonSettings.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/CEDistrbutedApp.hpp"
#include "edge/ui/CEDistributedDialog.hpp"

BEGIN_MODEL(NECommonSettings::MODEL_NAME_DISTRIBUTED_CLIENT)

    BEGIN_REGISTER_THREAD( NEDistributedApp::THREAD_DISTRIBUTED )
        BEGIN_REGISTER_COMPONENT( NECommonSettings::COMP_NAME_DISTRIBUTED_CLIENT, CEConnectionServicing )
            REGISTER_DEPENDENCY( NECommonSettings::COMP_NAME_CENTRAL_SERVER )
        END_REGISTER_COMPONENT( NECommonSettings::COMP_NAME_DISTRIBUTED_CLIENT )
    END_REGISTER_THREAD( NEDistributedApp::THREAD_DISTRIBUTED )

END_MODEL(NECommonSettings::MODEL_NAME_DISTRIBUTED_CLIENT)

DEF_TRACE_SCOPE( distrbutedapp_CEConnectionServicing_CreateComponent );
DEF_TRACE_SCOPE( distrbutedapp_CEConnectionServicing_DeleteComponent );

CEComponent * CEConnectionServicing::CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner )
{
    TRACE_SCOPE( distrbutedapp_CEConnectionServicing_CreateComponent );
    return new CEConnectionServicing( owner, entry.mRoleName.GetBuffer() );
}

void CEConnectionServicing::DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry )
{
    TRACE_SCOPE( distrbutedapp_CEConnectionServicing_DeleteComponent );
    delete (&compObject);
}

CEConnectionServicing::CEConnectionServicing( CEComponentThread & masterThread, const char * const roleName )
    : CEComponent       ( masterThread, roleName )
{
}

CEConnectionServicing::~CEConnectionServicing( void )
{
}

void CEConnectionServicing::StartupComponent( CEComponentThread & comThread )
{
    CEComponent::StartupComponent(comThread);
    CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceStartup, 1, reinterpret_cast<LPARAM>(this) );
}

void CEConnectionServicing::ShutdownComponent( CEComponentThread & comThread )
{
    CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceStartup, 0, 0 );
    CEComponent::ShutdownComponent(comThread);
}
