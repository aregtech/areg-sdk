
/************************************************************************
 * \file            edge/services/CEDirectChatService.cpp
 * \brief           The direct connection service client component
 ************************************************************************/

#include "edge/res/stdafx.h"
#include "edge/services/CEDirectChatService.hpp"
#include "edge/services/CEDirectConnectionClient.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/ui/CEPageConnections.hpp"
#include "edge/ui/CEPageChat.hpp"
#include "edge/ui/CEDistributedDialog.hpp"

DEF_TRACE_SCOPE( distrbutedapp_CEDirectChatService_StartupComponent );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectChatService_ShutdownComponent );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestChatJoin );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestMessageSend );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestMessageType );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestChatLeave );

NERegistry::CEModel CEDirectChatService::GetModel( const NEDirectMessager::sParticipant & initiator
                                                 , const NEDirectMessager::ListParticipants & listParticipants
                                                 , const NEMemory::uAlign data)
{
    CEString    roleName    = NEDistributedApp::getDirectMessagingRole( initiator.nickName, initiator.cookie, initiator.sessionId, true );
    CEString    threadName  = NEDistributedApp::PREFIX_TRHEAD + roleName;
    CEString    modelName   = NEDistributedApp::PREFIX_MODEL  + roleName;

    uint32_t count = listParticipants.GetSize();
    NERegistry::CEDependencyEntry depedency(roleName);
    NERegistry::CEDependencyList listDependencies( depedency );
    
    for ( uint32_t i = 0; i < count; ++i )
    {
        const NEDirectConnection::sParticipant & participant = listParticipants[i];
        NERegistry::CEDependencyEntry entry( NEDistributedApp::getConnectionServiceRole( participant.nickName, participant.cookie ) );
        listDependencies.Add( entry );
    }

    NERegistry::CEServiceEntry          serviceEntry( NEDirectMessager::ServiceName, NEDirectMessager::InterfaceVersion );
    NERegistry::CEServiceList           listServices( serviceEntry );
    NERegistry::CEComponentEntry        componentEntry( threadName, roleName, &CEDirectChatService::CreateComponent, CEDirectChatService::DeleteComponent, listServices, listDependencies, NERegistry::CEWorkerThreadList( ) );
    componentEntry.SetComponentData(data);
    NERegistry::CEComponentList         componentList( componentEntry );
    NERegistry::CEComponentThreadEntry  threadEntry( threadName, componentList );
    NERegistry::CEComponentThreadList   threadList( threadEntry );
    NERegistry::CEModel                 model( modelName, threadList );

    return model;
}

CEComponent * CEDirectChatService::CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner )
{
    CEPageChat * page = reinterpret_cast<CEPageChat *>(entry.GetComponentData().alignClsPtr.mElement);
    ASSERT( page != NULL );
    return ( page != NULL ? new CEDirectChatService(owner, entry, static_cast<CEChatPrticipantHandler &>(*page) ) : NULL );
}

void CEDirectChatService::DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry )
{
    delete (&compObject);
}

CEDirectChatService::CEDirectChatService( CEComponentThread & masterThread, const NERegistry::CEComponentEntry & entry, CEChatPrticipantHandler & handlerParticipant )
    : CEComponent           ( masterThread, entry.mRoleName)
    , CEDirectMessagerStub  ( static_cast<CEComponent &>(self()) )

    , mPaticipantsHandler   ( handlerParticipant )
    , mListClients          ( )
    , mChatParticipant      ( static_cast<CEComponent &>(self()), entry.mRoleName, handlerParticipant )
{
}

CEDirectChatService::~CEDirectChatService( void )
{
}

void CEDirectChatService::StartupComponent( CEComponentThread & comThread )
{
    TRACE_SCOPE( distrbutedapp_CEDirectChatService_StartupComponent );
    CEComponent::StartupComponent(comThread);
    mPaticipantsHandler.SetConnectionService( this );

    const NEDirectConnection::sInitiator & initiator = mPaticipantsHandler.GetInitiator();
    const NEDirectConnection::ListParticipants & listParticipants = mPaticipantsHandler.GetParticipantList();
    for ( int i = 0; i < listParticipants.GetSize( ); ++ i )
    {
        const NEDirectConnection::sParticipant & target = listParticipants[i];
        if ( target != initiator )
        {
            CEDirectConnectionClient * client = new CEDirectConnectionClient( static_cast<CEComponent &>(self( )), mPaticipantsHandler, target );
            mListClients.Add( client );
        }
    }
}

void CEDirectChatService::ShutdownComponent( CEComponentThread & comThread )
{
    TRACE_SCOPE( distrbutedapp_CEDirectChatService_ShutdownComponent );
    mPaticipantsHandler.SetConnectionService( static_cast<CEDirectChatService *>(NULL) );

    for ( int i = 0; i < mListClients.GetSize(); ++ i )
    {
        CEDirectConnectionClient * client = mListClients[i];
        if ( client != NULL )
            delete client;
    }
    mListClients.RemoveAll();

    CEComponent::ShutdownComponent(comThread);
}

void CEDirectChatService::StartupServiceInterface( CEComponent & holder )
{
    CEDirectMessagerStub::StartupServiceInterface(holder);
    SetChatParticipants( NEDirectMessager::ListParticipants() );
}

void CEDirectChatService::RequestChatJoin( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect )
{
    TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestChatJoin );
    if ( mPaticipantsHandler.ParticipantExist(participant) )
    {
        bool newParticipant = false;
        NEDirectMessager::ListParticipants & chatParticipants = GetChatParticipants();
        if ( chatParticipants.ElementExist(participant, 0) == false )
        {
            chatParticipants.Add(participant);
            newParticipant = true;
        }

        ResponseChatJoin(true, chatParticipants, timeConnect, CEDateTime::GetNow(false) );
        if ( newParticipant )
        {
            BroadcastParticipantJoined( participant, timeConnect );
            NotifyChatParticipantsUpdated( );
        }
    }
    else
    {
        ResponseChatJoin( false, NEDirectMessager::ListParticipants(), timeConnect, CEDateTime() );
    }
}

void CEDirectChatService::RequestMessageSend( const NEDirectMessager::sParticipant & participant, const CEString & msgText, const CEDateTime & timeSent )
{
    TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestMessageSend );
    const NEDirectMessager::ListParticipants & chatParticipants = GetChatParticipants( );
    if ( chatParticipants.ElementExist(participant, 0) )
    {
        BroadcastMessageSent(participant, msgText, timeSent );
        BroadcastMessageTyped( participant, CEString::EmptyString );
    }
}

void CEDirectChatService::RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText )
{
    TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestMessageType );
    const NEDirectMessager::ListParticipants & chatParticipants = GetChatParticipants( );
    if ( chatParticipants.ElementExist( participant, 0 ) )
    {
        BroadcastMessageTyped( participant, msgText );
    }
}

void CEDirectChatService::RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave )
{
    TRACE_SCOPE( distrbutedapp_CEDirectChatService_RequestChatLeave );
    NEDirectMessager::ListParticipants & chatParticipants = GetChatParticipants( );
    if ( chatParticipants.RemoveElement( participant, 0 ) )
    {
        BroadcastParticipantLeft( participant, timeLeave );
        NotifyChatParticipantsUpdated( );
    }
}
