
/************************************************************************
 * \file            chatter/services/DirectChatService.cpp
 * \brief           The direct connection service client component
 ************************************************************************/

#include "chatter/res/stdafx.h"
#include "chatter/services/DirectChatService.hpp"
#include "chatter/services/DirectConnectionClient.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/ui/PageConnections.hpp"
#include "chatter/ui/PageChat.hpp"
#include "chatter/ui/DistributedDialog.hpp"
#include "areg/component/ComponentLoader.hpp"

DEF_LOG_SCOPE( chatter_DirectChatService_StartupComponent );
DEF_LOG_SCOPE( chatter_DirectChatService_ShutdownComponent );
DEF_LOG_SCOPE( chatter_DirectChatService_RequestChatJoin );
DEF_LOG_SCOPE( chatter_DirectChatService_RequestMessageSend );
DEF_LOG_SCOPE( chatter_DirectChatService_RequestMessageType );
DEF_LOG_SCOPE( chatter_DirectChatService_RequestChatLeave );

areg::Model DirectChatService::GetModel( const DirectMessager::Participant & initiator
                                             , const DirectMessager::ListParticipants & listParticipants
                                             , std::any data)
{
    areg::String    roleName    = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, true );
    areg::String    threadName  = NEDistributedApp::PREFIX_TRHEAD + roleName;
    areg::String    modelName   = NEDistributedApp::PREFIX_MODEL  + roleName;

    uint32_t count = listParticipants.getSize();
    areg::DependencyEntry depedency(roleName);
    areg::DependencyList listDependencies( depedency );
    
    for ( uint32_t i = 0; i < count; ++i )
    {
        const DirectConnection::Participant & participant = listParticipants[i];
        areg::DependencyEntry entry( NEDistributedApp::getConnectionServiceRole( participant.nickName, participant.cookie ) );
        listDependencies.mListDependencies.add( entry );
    }

    areg::ServiceEntry          serviceEntry( DirectMessager::ServiceName, DirectMessager::InterfaceVersion );
    areg::ServiceList           listServices( serviceEntry );
    areg::ComponentEntry        componentEntry( threadName, roleName
                                                    , FUNC_CREATE_COMP(DirectChatService)
                                                    , FUNC_DELETE_COMP
                                                    , listServices, listDependencies, areg::WorkerThreadList( ) );
    componentEntry.setData(data);
    areg::ComponentList         componentList( componentEntry );
    areg::ComponentThreadEntry  threadEntry( threadName, componentList );
    areg::ComponentThreadList   threadList( threadEntry );
    areg::Model                 model( modelName, threadList );

    return model;
}

DirectChatService::DirectChatService( const areg::ComponentEntry & entry, ComponentThread & ownerThread)
    : Component           ( entry, ownerThread )
    , DirectMessagerStub  ( static_cast<Component &>(self()) )

    , mPaticipantsHandler   (std::any_cast<ChatPrticipantHandler*>(entry.getData()))
    , mListClients          ( )
    , mChatParticipant      ( static_cast<Component &>(self()), entry.mRoleName, mPaticipantsHandler)
{
}

DirectChatService::~DirectChatService()
{
    _clearList();
}

void DirectChatService::startupComponent( ComponentThread & comThread )
{
    LOG_SCOPE( chatter_DirectChatService_StartupComponent );
    Component::startupComponent(comThread);
    mPaticipantsHandler->SetConnectionService( this );

    const DirectConnection::sInitiator & initiator = mPaticipantsHandler->GetInitiator();
    const DirectConnection::ListParticipants & listParticipants = mPaticipantsHandler->GetParticipantList();
    uint32_t count {listParticipants.getSize( )};
    for (uint32_t i = 0; i < count; ++ i )
    {
        const DirectConnection::Participant & target = listParticipants[i];
        if ( target != initiator )
        {
            DirectConnectionClient * client = new DirectConnectionClient( static_cast<Component &>(self( )), mPaticipantsHandler, target );
            mListClients.add( client );
        }
    }
}

void DirectChatService::shutdownComponent( ComponentThread & comThread )
{
    LOG_SCOPE( chatter_DirectChatService_ShutdownComponent );
    mPaticipantsHandler->SetConnectionService( nullptr );
    
    _clearList();    
    Component::shutdownComponent(comThread);
}

void DirectChatService::startupServiceInterface( Component & holder )
{
    DirectMessagerStub::startupServiceInterface(holder);
    setChatParticipants( DirectMessager::ListParticipants() );
}

void DirectChatService::requestChatJoin( const DirectMessager::Participant & participant, const DateTime & timeConnect )
{
    LOG_SCOPE( chatter_DirectChatService_RequestChatJoin );
    if ( mPaticipantsHandler->ParticipantExist(participant) )
    {
        bool newParticipant = false;
        DirectMessager::ListParticipants & chatParticipants = getChatParticipants();
        if ( chatParticipants.contains(participant, 0) == false )
        {
            chatParticipants.add(participant);
            newParticipant = true;
        }

        responseChatJoin(true, chatParticipants, timeConnect, DateTime::getNow() );
        if ( newParticipant )
        {
            broadcastParticipantJoined( participant, timeConnect );
            notifyChatParticipantsUpdated( );
        }
    }
    else
    {
        responseChatJoin( false, DirectMessager::ListParticipants(), timeConnect, DateTime() );
    }
}

void DirectChatService::requestMessageSend( const DirectMessager::Participant & participant, const areg::String & msgText, const DateTime & timeSent )
{
    LOG_SCOPE( chatter_DirectChatService_RequestMessageSend );
    const DirectMessager::ListParticipants & chatParticipants = getChatParticipants( );
    if ( chatParticipants.contains(participant, 0) )
    {
        broadcastMessageSent(participant, msgText, timeSent );
        broadcastMessageTyped( participant, areg::String::getEmptyString() );
    }
}

void DirectChatService::requestMessageType( const DirectMessager::Participant & participant, const areg::String & msgText )
{
    LOG_SCOPE( chatter_DirectChatService_RequestMessageType );
    const DirectMessager::ListParticipants & chatParticipants = getChatParticipants( );
    if ( chatParticipants.contains( participant, 0 ) )
    {
        broadcastMessageTyped( participant, msgText );
    }
}

void DirectChatService::requestChatLeave( const DirectMessager::Participant & participant, const DateTime & timeLeave )
{
    LOG_SCOPE( chatter_DirectChatService_RequestChatLeave );
    DirectMessager::ListParticipants & chatParticipants = getChatParticipants( );
    if ( chatParticipants.removeElem( participant, 0 ) )
    {
        broadcastParticipantLeft( participant, timeLeave );
        notifyChatParticipantsUpdated( );
    }
}

inline void DirectChatService::_clearList()
{
    uint32_t count{ mListClients.getSize() };
    for (uint32_t i = 0; i < count; ++ i )
    {
        DirectConnectionClient * client = mListClients[i];
        delete client;
    }
    
    mListClients.clear();
}
