/************************************************************************
 * \file            edge/services/CEChatParticipantService.cpp
 * \brief           The chat participant service
 ************************************************************************/
#include "edge/res/stdafx.h"
#include "edge/services/CEChatParticipantService.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "edge/ui/CEPageChat.hpp"

CEComponent * CEChatParticipantService::CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner )
{
    CEPageChat * page = reinterpret_cast<CEPageChat *>(entry.GetComponentData( ).alignClsPtr.mElement);
    ASSERT( page != NULL );
    return (page != NULL ? new CEChatParticipantService( owner, entry, static_cast<CEChatPrticipantHandler &>(*page) ) : NULL);
}

void CEChatParticipantService::DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry )
{
    delete (&compObject);
}

NERegistry::CEModel CEChatParticipantService::GetModel( const NEDirectMessager::sParticipant & initiator, const NEDirectMessager::ListParticipants & listParticipants, NEMemory::uAlign data )
{
    CEString    serviceName = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, true );
    CEString    roleName    = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, false );
    CEString    threadName  = NEDistributedApp::PREFIX_TRHEAD   + roleName;
    CEString    modelName   = NEDistributedApp::PREFIX_MODEL    + roleName;

    NERegistry::CEDependencyEntry       dependency(serviceName);
    NERegistry::CEDependencyList        listDependencies( dependency);
    NERegistry::CEComponentEntry        componentEntry( threadName, roleName, &CEChatParticipantService::CreateComponent, CEChatParticipantService::DeleteComponent, NERegistry::CEServiceList( ), listDependencies, NERegistry::CEWorkerThreadList( ) );
    componentEntry.SetComponentData( data );
    NERegistry::CEComponentList         componentList( componentEntry );
    NERegistry::CEComponentThreadEntry  threadEntry( threadName, componentList );
    NERegistry::CEComponentThreadList   threadList( threadEntry );
    NERegistry::CEModel                 model( modelName, threadList );

    return model;
}

CEChatParticipantService::CEChatParticipantService( CEComponentThread & masterThread, const NERegistry::CEComponentEntry & entry, CEChatPrticipantHandler & handlerParticipants )
    : CEComponent       ( masterThread, entry.mRoleName )

    , mChatParticipant  ( static_cast<CEComponent &>(self()), entry.mDependencyServices[0].mRoleName, handlerParticipants )
{
}

CEChatParticipantService::~CEChatParticipantService( void )
{
}

void CEChatParticipantService::StartupComponent( CEComponentThread & comThread )
{
    CEComponent::StartupComponent(comThread);
}

void CEChatParticipantService::ShutdownComponent( CEComponentThread & comThread )
{
    CEComponent::ShutdownComponent(comThread);
}
