/************************************************************************
 * \file            chatter/services/ChatParticipantService.cpp
 * \brief           The chat participant service
 ************************************************************************/
#include "chatter/res/stdafx.h"
#include "chatter/services/ChatParticipantService.hpp"
#include "areg/component/ComponentThread.hpp"
#include "chatter/ui/PageChat.hpp"

NERegistry::Model ChatParticipantService::GetModel( const NEDirectMessager::sParticipant & initiator, const NEDirectMessager::ListParticipants & /* listParticipants */, NEMemory::uAlign data )
{
    String    serviceName = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, true );
    String    roleName    = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, false );
    String    threadName  = NEDistributedApp::PREFIX_TRHEAD   + roleName;
    String    modelName   = NEDistributedApp::PREFIX_MODEL    + roleName;

    NERegistry::DependencyEntry       dependency(serviceName);
    NERegistry::DependencyList        listDependencies( dependency);
    NERegistry::ComponentEntry        componentEntry( threadName, roleName
                                                    , ([](const NERegistry::ComponentEntry& e, ComponentThread& t) -> Component* {return new ChatParticipantService(e, t); })
                                                    , ([](Component& c, const NERegistry::ComponentEntry& /*e*/) -> void {delete& c; })
                                                    , NERegistry::ServiceList( ), listDependencies, NERegistry::WorkerThreadList( ) );
    componentEntry.setComponentData( data );
    NERegistry::ComponentList         componentList( componentEntry );
    NERegistry::ComponentThreadEntry  threadEntry( threadName, componentList );
    NERegistry::ComponentThreadList   threadList( threadEntry );
    NERegistry::Model                 model( modelName, threadList );

    return model;
}

ChatParticipantService::ChatParticipantService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread )
    : Component         ( entry, ownerThread )

    , mChatParticipant  ( static_cast<Component &>(self()), entry.mDependencyServices[0].mRoleName, *reinterpret_cast<ChatPrticipantHandler*>(entry.getComponentData().alignClsPtr.mElement))
{
}

void ChatParticipantService::startupComponent( ComponentThread & comThread )
{
    Component::startupComponent(comThread);
}

void ChatParticipantService::shutdownComponent( ComponentThread & comThread )
{
    Component::shutdownComponent(comThread);
}
