
/************************************************************************
 * \file            edge/services/CEDirectConnectionService.cpp
 * \brief           The messaging service client object
 ************************************************************************/
#include "edge/res/stdafx.h"
#include "edge/services/CEDirectConnectionService.hpp"
#include "edge/ui/CEDistributedDialog.hpp"
#include "edge/NEDistributedApp.hpp"
#include "areg/src/trace/GETrace.h"

DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_CreateComponent );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_DeleteComponent );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_StartupServiceInterface );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_ShutdownServiceIntrface );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_ClientConnected );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestConnectoinSetup );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestAddParticipant );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestRemoveParticipant );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestCloseConnection );

CEDirectConnectionService * CEDirectConnectionService::mService = NULL;

CEComponent * CEDirectConnectionService::CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_CreateComponent );
    return new CEDirectConnectionService( owner, entry.mRoleName.GetBuffer(), entry.GetComponentData() );
}

void CEDirectConnectionService::DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_DeleteComponent );
    delete (&compObject);
}

CEDirectConnectionService * CEDirectConnectionService::GetService( void )
{
    return CEDirectConnectionService::mService;
}

CEString CEDirectConnectionService::GetGeneratedService( const CEString & nickName, uint32_t cookie )
{
    return NEDistributedApp::getConnectionServiceRole(nickName, cookie);
}

NERegistry::CEModel CEDirectConnectionService::GetModel( const CEString & nickName, uint32_t cookie, const NEMemory::uAlign data )
{
    CEString    roleName    = CEDirectConnectionService::GetGeneratedService(nickName, cookie);
    CEString    threadName  = NEDistributedApp::PREFIX_TRHEAD   + roleName;
    CEString    modelName   = NEDistributedApp::PREFIX_MODEL    + roleName;

    NERegistry::CEServiceEntry          serviceEntry( NEDirectConnection::ServiceName, NEDirectConnection::InterfaceVersion );
    NERegistry::CEServiceList           serviceList( serviceEntry );
    NERegistry::CEComponentEntry        componentEntry(threadName, roleName, &CEDirectConnectionService::CreateComponent, CEDirectConnectionService::DeleteComponent, serviceList, NERegistry::CEDependencyList(), NERegistry::CEWorkerThreadList());
    componentEntry.SetComponentData(data);
    NERegistry::CEComponentList         componentList(componentEntry);
    NERegistry::CEComponentThreadEntry  threadEntry(threadName, componentList);
    NERegistry::CEComponentThreadList   threadList( threadEntry );
    NERegistry::CEModel                 model(modelName, threadList);
    
    return model;
}

CEDirectConnectionService::CEDirectConnectionService( CEComponentThread & masterThread, const char * const roleName, NEMemory::uAlign data )
    : CEComponent               ( masterThread, roleName )
    , CEDirectConnectionStub    ( static_cast<CEComponent &>(self()) )

    , mNickName                 ( reinterpret_cast<CEPageConnections *>(data.alignClsPtr.mElement)->GetRegisteredName() )
    , mCookie                   ( reinterpret_cast<CEPageConnections *>(data.alignClsPtr.mElement)->GetRegisteredCookie() )
{
    CEDirectConnectionService::mService = this;
}

CEDirectConnectionService::~CEDirectConnectionService( )
{
    CEDirectConnectionService::mService = NULL;
}

void CEDirectConnectionService::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_StartupServiceInterface );
    CEDirectConnectionStub::StartupServiceInterface(holder);
}

void CEDirectConnectionService::ShutdownServiceIntrface ( CEComponent & holder )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_ShutdownServiceIntrface );
    CEDirectConnectionStub::ShutdownServiceIntrface( holder );
}

void CEDirectConnectionService::ClientConnected( const CEProxyAddress & client, bool isConnected )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_ClientConnected );
    CEDirectConnectionStub::ClientConnected(client, isConnected);
}

inline bool CEDirectConnectionService::isInitiatorValid( const NEDirectConnection::sInitiator & initiator ) const
{
    return (    (initiator.nickName.IsEmpty()   == false                            ) && 
                (initiator.cookie               != NEDirectConnection::InvalidCookie) &&
                (initiator.sessionId            != NEDirectConnection::InvalidSession) );
}

inline bool CEDirectConnectionService::exists( const NEDirectConnection::sInitiator & initiator ) const
{
    return (GetInitiatedConnections().Find(initiator) != static_cast<MAPPOS>(NULL));
}

uint64_t CEDirectConnectionService::getSession( const NEDirectConnection::ListParticipants & listParticipants )
{
    uint64_t result = NEDirectConnection::InvalidSession;
    for ( int i = 0; i < listParticipants.GetSize(); ++ i )
    {
        const NEDirectConnection::sParticipant & participant = listParticipants[i];
        if ( (participant.nickName == mNickName) && (participant.cookie == mCookie) )
        {
            result = participant.sessionId;
            break;
        }
    }
    return result;
}

void CEDirectConnectionService::RequestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestConnectoinSetup );

    ASSERT(mNickName.IsEmpty() == false);
    ASSERT(mCookie != NEDirectConnection::InvalidCookie);

    NEDirectConnection::sParticipant participant;
    participant.nickName = mNickName;
    participant.cookie   = mCookie;
    participant.sessionId= NEDirectConnection::InvalidSession;

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == false )
        {
            TRACE_DBG("[ %s ] at timestampt [ %s ] initiated chat with [ %d ] clients. Setting up chat."
                            , initiator.nickName.GetBuffer()
                            , CEDateTime(initiator.sessionId).FormatTime().GetBuffer()
                            , listParticipants.GetSize() );

            uint64_t sessionID = getSession(listParticipants);
            participant.sessionId= sessionID;

            if ( sessionID != NEDirectConnection::InvalidSession )
            {
                NEDirectConnection::MapParticipants & mapParticipants = GetInitiatedConnections( );
                mapParticipants.SetKey( initiator, listParticipants );
                ResponseConnectoinSetup( true, participant, initiator, listParticipants );
                NEDirectConnection::sInitiator      * wParam    = new NEDirectConnection::sInitiator(initiator);
                NEDirectConnection::ListParticipants * lParam   = new NEDirectConnection::ListParticipants(listParticipants);

                CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdSetDirectConnection, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
            }
            else
            {
                TRACE_DBG("[ %s ] with cookie [ %u ] could not find session ID. Ignoring direct connection setup.", mNickName.GetBuffer(), mCookie);
                ResponseConnectoinSetup( false, participant, initiator, NEDirectConnection::ListParticipants() );
            }
        }
        else
        {
            const NEDirectConnection::MapParticipants &  mapParticipants = GetInitiatedConnections( );
            const NEDirectConnection::ListParticipants & listParticipants= mapParticipants.GetAt(initiator);
            TRACE_WARN("[ %s ] at timestampt [ %s ] has already initiated chat with [ %d ] clients. Ignoring chat setup."
                            , participant.nickName.String()
                            , CEDateTime(initiator.sessionId).FormatTime().String()
                            , mapParticipants.GetSize() );
            participant.sessionId = getSession(listParticipants);
            ResponseConnectoinSetup( true, participant, initiator, listParticipants );
        }
    }
    else
    {
        TRACE_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]", participant.nickName.String(), participant.cookie, participant.sessionId);
        ResponseConnectoinSetup( false, participant, initiator, NEDirectConnection::ListParticipants( ) );
    }
}

void CEDirectConnectionService::RequestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestAddParticipant );

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == true )
        {
            TRACE_DBG("[ %s ] at timestampt [ %s ] is adding chat participants of [ %d ] clients."
                            , initiator.nickName.GetBuffer()
                            , CEDateTime(initiator.sessionId).FormatTime().GetBuffer()
                            , listParticipants.GetSize() );

            NEDirectConnection::MapParticipants & mapParticipants = GetInitiatedConnections( );
            NEDirectConnection::ListParticipants listRegistered (mapParticipants.GetAt( initiator ));
            listRegistered.Append(listParticipants);
            mapParticipants[initiator] = listRegistered;
            ResponseAddParticipant( true, listRegistered );
            NEDirectConnection::sInitiator      * wParam = new NEDirectConnection::sInitiator(initiator);
            NEDirectConnection::ListParticipants* lParam = new NEDirectConnection::ListParticipants(listRegistered);
            CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdDirectConnectionAdd, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
        }
        else
        {
            TRACE_WARN("[ %s ] at timestampt [ %s ] has no participants. Ignoring request to add participants."
                            , initiator.nickName.GetBuffer()
                            , CEDateTime(initiator.sessionId).FormatTime().GetBuffer() );
            ResponseAddParticipant(false, NEDirectConnection::ListParticipants());
        }
    }
    else
    {
        TRACE_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]", initiator.nickName.GetBuffer(), initiator.cookie, initiator.sessionId);
        ResponseAddParticipant( false, NEDirectConnection::ListParticipants( ) );
    }
}

void CEDirectConnectionService::RequestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestRemoveParticipant );

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == true )
        {
            TRACE_DBG("[ %s ] at timestampt [ %s ] is removing chat participants of [ %d ] clients."
                            , initiator.nickName.GetBuffer()
                            , CEDateTime(initiator.sessionId).FormatTime().GetBuffer()
                            , listParticipants.GetSize() );

            NEDirectConnection::MapParticipants & mapParticpants = GetInitiatedConnections( );
            NEDirectConnection::ListParticipants listRegistered( mapParticpants.GetAt( initiator ) );
            for ( int i = 0; i < listParticipants.GetSize(); ++ i )
            {
                const NEDirectConnection::sParticipant & particpant = listParticipants[i];
                for ( int j = 0; j < listRegistered.GetSize(); ++ j )
                {
                    if ( particpant == listRegistered[j] )
                    {
                        listRegistered.RemoveAt(j);
                        break;
                    }
                }
            }
            if ( listRegistered.IsEmpty() )
            {
                mapParticpants.RemoveKey(initiator);
            }
            else
            {
                mapParticpants.SetKey(initiator, listRegistered);
            }
            ResponseRemoveParticipant( true, listRegistered );
            NEDirectConnection::sInitiator      * wParam = new NEDirectConnection::sInitiator(initiator);
            NEDirectConnection::ListParticipants* lParam = new NEDirectConnection::ListParticipants(listRegistered);
            CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdDirectConnectionRemove, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
        }
        else
        {
            TRACE_WARN("[ %s ] at timestampt [ %s ] has no participants. Ignoring request to remove participants."
                            , initiator.nickName.GetBuffer()
                            , CEDateTime(initiator.sessionId).FormatTime().GetBuffer() );
            ResponseRemoveParticipant(false, NEDirectConnection::ListParticipants());
        }
    }
    else
    {
        TRACE_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]", initiator.nickName.GetBuffer(), initiator.cookie, initiator.sessionId);
        ResponseRemoveParticipant( false, NEDirectConnection::ListParticipants( ) );
    }
}

void CEDirectConnectionService::RequestCloseConnection( const NEDirectConnection::sInitiator & initiator )
{
    TRACE_SCOPE( distrbutedapp_CEDirectConnectionService_RequestCloseConnection );
    NEDirectConnection::MapParticipants & mapParticipants = GetInitiatedConnections( );
    mapParticipants.RemoveKey(initiator);
    NEDirectConnection::sInitiator      * wParam = new NEDirectConnection::sInitiator( initiator );
    NEDirectConnection::ListParticipants* lParam = NULL;
    CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdDirectConnectionClose, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
}
