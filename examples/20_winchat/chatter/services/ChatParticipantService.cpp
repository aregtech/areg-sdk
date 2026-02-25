/************************************************************************
 * \file            chatter/services/ChatParticipantService.cpp
 * \brief           The chat participant service
 ************************************************************************/
#include "chatter/res/stdafx.h"
#include "chatter/services/ChatParticipantService.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "chatter/ui/PageChat.hpp"

areg::Model ChatParticipantService::GetModel( const DirectMessager::Participant & initiator, const DirectMessager::ListParticipants & /* listParticipants */, std::any data )
{
    areg::String    serviceName = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, true );
    areg::String    roleName    = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, false );
    areg::String    threadName  = NEDistributedApp::PREFIX_TRHEAD   + roleName;
    areg::String    modelName   = NEDistributedApp::PREFIX_MODEL    + roleName;

    areg::DependencyEntry       dependency(serviceName);
    areg::DependencyList        listDependencies( dependency);
    areg::ComponentEntry        componentEntry( threadName, roleName
                                                    , FUNC_CREATE_COMP(ChatParticipantService)
                                                    , FUNC_DELETE_COMP
                                                    , areg::ServiceList( ), listDependencies, areg::WorkerThreadList( ) );
    componentEntry.setData( data );
    areg::ComponentList         componentList( componentEntry );
    areg::ComponentThreadEntry  threadEntry( threadName, componentList );
    areg::ComponentThreadList   threadList( threadEntry );
    areg::Model                 model( modelName, threadList );

    return model;
}

ChatParticipantService::ChatParticipantService( const areg::ComponentEntry & entry, ComponentThread & ownerThread )
    : Component         ( entry, ownerThread )

    , mChatParticipant  ( static_cast<Component &>(self()), entry.mDependencyServices[0].mRoleName, std::any_cast<ChatPrticipantHandler*>(entry.getData()) )
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
