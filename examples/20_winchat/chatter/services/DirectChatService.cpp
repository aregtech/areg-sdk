
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

    uint32_t count = listParticipants.size();
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
    componentEntry.set_data(data);
    areg::ComponentList         componentList( componentEntry );
    areg::ComponentThreadEntry  threadEntry( threadName, componentList );
    areg::ComponentThreadList   threadList( threadEntry );
    areg::Model                 model( modelName, threadList );

    return model;
}

DirectChatService::DirectChatService( const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread)
    : areg::Component           ( entry, ownerThread )
    , DirectMessagerProviderBase  ( static_cast<areg::Component &>(self()) )

    , mPaticipantsHandler   (std::any_cast<ChatPrticipantHandler*>(entry.data()))
    , mListClients          ( )
    , mChatParticipant      ( static_cast<areg::Component &>(self()), entry.mRoleName, mPaticipantsHandler)
{
}

DirectChatService::~DirectChatService()
{
    _clearList();
}

void DirectChatService::startup_component( areg::ComponentThread & comThread )
{
    LOG_SCOPE( chatter_DirectChatService_StartupComponent );
    areg::Component::startup_component(comThread);
    mPaticipantsHandler->SetConnectionService( this );

    const DirectConnection::sInitiator & initiator = mPaticipantsHandler->GetInitiator();
    const DirectConnection::ListParticipants & listParticipants = mPaticipantsHandler->GetParticipantList();
    uint32_t count {listParticipants.size( )};
    for (uint32_t i = 0; i < count; ++ i )
    {
        const DirectConnection::Participant & target = listParticipants[i];
        if ( target != initiator )
        {
            DirectConnectionClient * client = new DirectConnectionClient( static_cast<areg::Component &>(self( )), mPaticipantsHandler, target );
            mListClients.add( client );
        }
    }
}

void DirectChatService::shutdown_component( areg::ComponentThread & comThread )
{
    LOG_SCOPE( chatter_DirectChatService_ShutdownComponent );
    mPaticipantsHandler->SetConnectionService( nullptr );
    
    _clearList();    
    areg::Component::shutdown_component(comThread);
}

void DirectChatService::startup_service_interface( areg::Component & holder )
{
    DirectMessagerProviderBase::startup_service_interface(holder);
    set_chat_participants( DirectMessager::ListParticipants() );
}

void DirectChatService::request_chat_join( const DirectMessager::Participant & participant, const areg::DateTime & timeConnect )
{
    LOG_SCOPE( chatter_DirectChatService_RequestChatJoin );
    if ( mPaticipantsHandler->ParticipantExist(participant) )
    {
        bool newParticipant = false;
        DirectMessager::ListParticipants & chatParticipants = chat_participants();
        if ( chatParticipants.contains(participant, 0) == false )
        {
            chatParticipants.add(participant);
            newParticipant = true;
        }

        response_chat_join(true, chatParticipants, timeConnect, areg::DateTime::now() );
        if ( newParticipant )
        {
            broadcast_participant_joined( participant, timeConnect );
            notify_chat_participants_updated( );
        }
    }
    else
    {
        response_chat_join( false, DirectMessager::ListParticipants(), timeConnect, areg::DateTime() );
    }
}

void DirectChatService::request_message_send( const DirectMessager::Participant & participant, const areg::String & msgText, const areg::DateTime & timeSent )
{
    LOG_SCOPE( chatter_DirectChatService_RequestMessageSend );
    const DirectMessager::ListParticipants & chatParticipants = chat_participants( );
    if ( chatParticipants.contains(participant, 0) )
    {
        broadcast_message_sent(participant, msgText, timeSent );
        broadcast_message_typed( participant, areg::String::empty_string() );
    }
}

void DirectChatService::request_message_type( const DirectMessager::Participant & participant, const areg::String & msgText )
{
    LOG_SCOPE( chatter_DirectChatService_RequestMessageType );
    const DirectMessager::ListParticipants & chatParticipants = chat_participants( );
    if ( chatParticipants.contains( participant, 0 ) )
    {
        broadcast_message_typed( participant, msgText );
    }
}

void DirectChatService::request_chat_leave( const DirectMessager::Participant & participant, const areg::DateTime & timeLeave )
{
    LOG_SCOPE( chatter_DirectChatService_RequestChatLeave );
    DirectMessager::ListParticipants & chatParticipants = chat_participants( );
    if ( chatParticipants.remove_elem( participant, 0 ) )
    {
        broadcast_participant_left( participant, timeLeave );
        notify_chat_participants_updated( );
    }
}

inline void DirectChatService::_clearList()
{
    uint32_t count{ mListClients.size() };
    for (uint32_t i = 0; i < count; ++ i )
    {
        DirectConnectionClient * client = mListClients[i];
        delete client;
    }
    
    mListClients.clear();
}
