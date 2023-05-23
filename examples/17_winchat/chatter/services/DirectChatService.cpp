
/************************************************************************
 * \file            chatter/services/DirectChatService.cpp
 * \brief           The direct connection service client component
 ************************************************************************/

#include "chatter/res/stdafx.h"
#include "chatter/services/DirectChatService.hpp"
#include "chatter/services/DirectConnectionClient.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/ui/PageConnections.hpp"
#include "chatter/ui/PageChat.hpp"
#include "chatter/ui/DistributedDialog.hpp"

DEF_TRACE_SCOPE( distrbutedapp_DirectChatService_StartupComponent );
DEF_TRACE_SCOPE( distrbutedapp_DirectChatService_ShutdownComponent );
DEF_TRACE_SCOPE( distrbutedapp_DirectChatService_RequestChatJoin );
DEF_TRACE_SCOPE( distrbutedapp_DirectChatService_RequestMessageSend );
DEF_TRACE_SCOPE( distrbutedapp_DirectChatService_RequestMessageType );
DEF_TRACE_SCOPE( distrbutedapp_DirectChatService_RequestChatLeave );

NERegistry::Model DirectChatService::GetModel( const NEDirectMessager::sParticipant & initiator
                                             , const NEDirectMessager::ListParticipants & listParticipants
                                             , const NEMemory::uAlign data)
{
    String    roleName    = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, true );
    String    threadName  = NEDistributedApp::PREFIX_TRHEAD + roleName;
    String    modelName   = NEDistributedApp::PREFIX_MODEL  + roleName;

    uint32_t count = listParticipants.getSize();
    NERegistry::DependencyEntry depedency(roleName);
    NERegistry::DependencyList listDependencies( depedency );
    
    for ( uint32_t i = 0; i < count; ++i )
    {
        const NEDirectConnection::sParticipant & participant = listParticipants[i];
        NERegistry::DependencyEntry entry( NEDistributedApp::getConnectionServiceRole( participant.nickName, participant.cookie ) );
        listDependencies.mListDependencies.add( entry );
    }

    NERegistry::ServiceEntry          serviceEntry( NEDirectMessager::ServiceName, NEDirectMessager::InterfaceVersion );
    NERegistry::ServiceList           listServices( serviceEntry );
    NERegistry::ComponentEntry        componentEntry( threadName, roleName, &DirectChatService::CreateComponent, DirectChatService::DeleteComponent, listServices, listDependencies, NERegistry::WorkerThreadList( ) );
    componentEntry.setComponentData(data);
    NERegistry::ComponentList         componentList( componentEntry );
    NERegistry::ComponentThreadEntry  threadEntry( threadName, componentList );
    NERegistry::ComponentThreadList   threadList( threadEntry );
    NERegistry::Model                 model( modelName, threadList );

    return model;
}

Component * DirectChatService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    PageChat * page = reinterpret_cast<PageChat *>(entry.getComponentData().alignClsPtr.mElement);
    ASSERT( page != nullptr );
    return ( page != nullptr ? new DirectChatService(entry, owner, static_cast<ChatPrticipantHandler &>(*page) ) : nullptr);
}

void DirectChatService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    delete (&compObject);
}

DirectChatService::DirectChatService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread, ChatPrticipantHandler & handlerParticipant )
    : Component           ( entry, ownerThread )
    , DirectMessagerStub  ( static_cast<Component &>(self()) )

    , mPaticipantsHandler   ( handlerParticipant )
    , mListClients          ( )
    , mChatParticipant      ( static_cast<Component &>(self()), entry.mRoleName, handlerParticipant )
{
}

void DirectChatService::startupComponent( ComponentThread & comThread )
{
    TRACE_SCOPE( distrbutedapp_DirectChatService_StartupComponent );
    Component::startupComponent(comThread);
    mPaticipantsHandler.SetConnectionService( this );

    const NEDirectConnection::sInitiator & initiator = mPaticipantsHandler.GetInitiator();
    const NEDirectConnection::ListParticipants & listParticipants = mPaticipantsHandler.GetParticipantList();
    for (uint32_t i = 0; i < listParticipants.getSize( ); ++ i )
    {
        const NEDirectConnection::sParticipant & target = listParticipants[i];
        if ( target != initiator )
        {
            DirectConnectionClient * client = new DirectConnectionClient( static_cast<Component &>(self( )), mPaticipantsHandler, target );
            mListClients.add( client );
        }
    }
}

void DirectChatService::shutdownComponent( ComponentThread & comThread )
{
    TRACE_SCOPE( distrbutedapp_DirectChatService_ShutdownComponent );
    mPaticipantsHandler.SetConnectionService( nullptr );

    for (uint32_t i = 0; i < mListClients.getSize(); ++ i )
    {
        DirectConnectionClient * client = mListClients[i];
        delete client;
    }
    mListClients.clear();

    Component::shutdownComponent(comThread);
}

void DirectChatService::startupServiceInterface( Component & holder )
{
    DirectMessagerStub::startupServiceInterface(holder);
    setChatParticipants( NEDirectMessager::ListParticipants() );
}

void DirectChatService::requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect )
{
    TRACE_SCOPE( distrbutedapp_DirectChatService_RequestChatJoin );
    if ( mPaticipantsHandler.ParticipantExist(participant) )
    {
        bool newParticipant = false;
        NEDirectMessager::ListParticipants & chatParticipants = getChatParticipants();
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
        responseChatJoin( false, NEDirectMessager::ListParticipants(), timeConnect, DateTime() );
    }
}

void DirectChatService::requestMessageSend( const NEDirectMessager::sParticipant & participant, const String & msgText, const DateTime & timeSent )
{
    TRACE_SCOPE( distrbutedapp_DirectChatService_RequestMessageSend );
    const NEDirectMessager::ListParticipants & chatParticipants = getChatParticipants( );
    if ( chatParticipants.contains(participant, 0) )
    {
        broadcastMessageSent(participant, msgText, timeSent );
        broadcastMessageTyped( participant, String::getEmptyString() );
    }
}

void DirectChatService::requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
{
    TRACE_SCOPE( distrbutedapp_DirectChatService_RequestMessageType );
    const NEDirectMessager::ListParticipants & chatParticipants = getChatParticipants( );
    if ( chatParticipants.contains( participant, 0 ) )
    {
        broadcastMessageTyped( participant, msgText );
    }
}

void DirectChatService::requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
{
    TRACE_SCOPE( distrbutedapp_DirectChatService_RequestChatLeave );
    NEDirectMessager::ListParticipants & chatParticipants = getChatParticipants( );
    if ( chatParticipants.removeElem( participant, 0 ) )
    {
        broadcastParticipantLeft( participant, timeLeave );
        notifyChatParticipantsUpdated( );
    }
}
