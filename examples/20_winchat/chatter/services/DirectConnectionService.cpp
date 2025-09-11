
/************************************************************************
 * \file            chatter/services/DirectConnectionService.cpp
 * \brief           The messaging service client object
 ************************************************************************/
#include "chatter/res/stdafx.h"
#include "chatter/services/DirectConnectionService.hpp"
#include "chatter/ui/DistributedDialog.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "areg/logging/GELog.h"

#include <Windows.h>

DEF_LOG_SCOPE( chatter_DirectConnectionService_requestConnectoinSetup );
DEF_LOG_SCOPE( chatter_DirectConnectionService_requestAddParticipant );
DEF_LOG_SCOPE( chatter_DirectConnectionService_requestRemoveParticipant );
DEF_LOG_SCOPE( chatter_DirectConnectionService_requestCloseConnection );

DirectConnectionService * DirectConnectionService::mService = nullptr;

DirectConnectionService * DirectConnectionService::GetService( void )
{
    return DirectConnectionService::mService;
}

String DirectConnectionService::GetGeneratedService( const String & nickName, uint32_t cookie )
{
    return NEDistributedApp::getConnectionServiceRole(nickName, cookie);
}

NERegistry::Model DirectConnectionService::GetModel( const String & nickName, uint32_t cookie, const NEMemory::uAlign data )
{
    String    roleName    = DirectConnectionService::GetGeneratedService(nickName, cookie);
    String    threadName  = NEDistributedApp::PREFIX_TRHEAD   + roleName;
    String    modelName   = NEDistributedApp::PREFIX_MODEL    + roleName;

    NERegistry::ServiceEntry          serviceEntry( NEDirectConnection::ServiceName, NEDirectConnection::InterfaceVersion );
    NERegistry::ServiceList           serviceList( serviceEntry );
    NERegistry::ComponentEntry        componentEntry(threadName, roleName
                                                    , ([](const NERegistry::ComponentEntry& e, ComponentThread& t) -> Component * {return new DirectConnectionService(e, t);})
                                                    , ([](Component& c, const NERegistry::ComponentEntry& /*e*/) -> void {delete& c; })
                                                    , serviceList, NERegistry::DependencyList(), NERegistry::WorkerThreadList());
    componentEntry.setComponentData(data);
    NERegistry::ComponentList         componentList(componentEntry);
    NERegistry::ComponentThreadEntry  threadEntry(threadName, componentList);
    NERegistry::ComponentThreadList   threadList( threadEntry );
    NERegistry::Model                 model(modelName, threadList);
    
    return model;
}

DirectConnectionService::DirectConnectionService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread )
    : Component             ( entry, ownerThread )
    , DirectConnectionStub  ( static_cast<Component &>(self()) )

    , mNickName             ( reinterpret_cast<PageConnections *>(entry.getComponentData().alignClsPtr.mElement)->GetRegisteredName() )
    , mCookie               ( reinterpret_cast<PageConnections *>(entry.getComponentData().alignClsPtr.mElement)->GetRegisteredCookie() )
{
    DirectConnectionService::mService = this;
}

DirectConnectionService::~DirectConnectionService( )
{
    DirectConnectionService::mService = nullptr;
}

inline bool DirectConnectionService::isInitiatorValid( const NEDirectConnection::sInitiator & initiator ) const
{
    return (    (initiator.nickName.isEmpty()   == false                            ) && 
                (initiator.cookie               != NEDirectConnection::InvalidCookie) &&
                (initiator.sessionId            != NEDirectConnection::InvalidSession) );
}

inline bool DirectConnectionService::exists( const NEDirectConnection::sInitiator & initiator ) const
{
    return (getInitiatedConnections().contains(initiator));
}

uint64_t DirectConnectionService::getSession( const NEDirectConnection::ListParticipants & listParticipants )
{
    uint64_t result = NEDirectConnection::InvalidSession;
    for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
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

void DirectConnectionService::requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestConnectoinSetup );

    ASSERT(mNickName.isEmpty() == false);
    ASSERT(mCookie != NEDirectConnection::InvalidCookie);

    NEDirectConnection::sParticipant participant;
    participant.nickName = mNickName;
    participant.cookie   = mCookie;
    participant.sessionId= NEDirectConnection::InvalidSession;

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == false )
        {
            LOG_DBG("[ %s ] at time-stamps [ %s ] initiated chat with [ %d ] clients. Setting up chat."
                            , initiator.nickName.getBuffer()
                            , DateTime(initiator.sessionId).formatTime().getBuffer()
                            , listParticipants.getSize() );

            uint64_t sessionID = getSession(listParticipants);
            participant.sessionId= sessionID;

            if ( sessionID != NEDirectConnection::InvalidSession )
            {
                NEDirectConnection::MapParticipants & mapParticipants = getInitiatedConnections( );
                mapParticipants.setAt( initiator, listParticipants );
                responseConnectoinSetup( true, participant, initiator, listParticipants );
                NEDirectConnection::sInitiator      * wParam    = new NEDirectConnection::sInitiator(initiator);
                NEDirectConnection::ListParticipants * lParam   = new NEDirectConnection::ListParticipants(listParticipants);

                DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdSetDirectConnection, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
            }
            else
            {
                LOG_DBG("[ %s ] with cookie [ %u ] could not find session ID. Ignoring direct connection setup.", mNickName.getBuffer(), mCookie);
                responseConnectoinSetup( false, participant, initiator, NEDirectConnection::ListParticipants() );
            }
        }
        else
        {
            const NEDirectConnection::MapParticipants &  mapParticipants = getInitiatedConnections( );
            const NEDirectConnection::ListParticipants & tempList = mapParticipants.getAt(initiator);
            LOG_WARN("[ %s ] at time-stamps [ %s ] has already initiated chat with [ %d ] clients. Ignoring chat setup."
                            , participant.nickName.getString()
                            , DateTime(initiator.sessionId).formatTime().getString()
                            , mapParticipants.getSize() );
            participant.sessionId = getSession(tempList);
            responseConnectoinSetup( true, participant, initiator, tempList );
        }
    }
    else
    {
        LOG_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]"
                    , participant.nickName.getString()
                    , participant.cookie
                    , participant.sessionId);
        responseConnectoinSetup( false, participant, initiator, NEDirectConnection::ListParticipants( ) );
    }
}

void DirectConnectionService::requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestAddParticipant );

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == true )
        {
            LOG_DBG("[ %s ] at time-stamps [ %s ] is adding chat participants of [ %d ] clients."
                            , initiator.nickName.getBuffer()
                            , DateTime(initiator.sessionId).formatTime().getBuffer()
                            , listParticipants.getSize() );

            NEDirectConnection::MapParticipants & mapParticipants = getInitiatedConnections( );
            NEDirectConnection::ListParticipants listRegistered (mapParticipants.getAt( initiator ));
            listRegistered.append(listParticipants);
            mapParticipants[initiator] = listRegistered;
            responseAddParticipant( true, listRegistered );
            NEDirectConnection::sInitiator      * wParam = new NEDirectConnection::sInitiator(initiator);
            NEDirectConnection::ListParticipants* lParam = new NEDirectConnection::ListParticipants(listRegistered);
            DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdDirectConnectionAdd, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
        }
        else
        {
            LOG_WARN("[ %s ] at time-stamps [ %s ] has no participants. Ignoring request to add participants."
                            , initiator.nickName.getBuffer()
                            , DateTime(initiator.sessionId).formatTime().getBuffer() );
            responseAddParticipant(false, NEDirectConnection::ListParticipants());
        }
    }
    else
    {
        LOG_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]", initiator.nickName.getString(), initiator.cookie, initiator.sessionId);
        responseAddParticipant( false, NEDirectConnection::ListParticipants( ) );
    }
}

void DirectConnectionService::requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestRemoveParticipant );

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == true )
        {
            LOG_DBG("[ %s ] at time-stamps [ %s ] is removing chat participants of [ %d ] clients."
                            , initiator.nickName.getBuffer()
                            , DateTime(initiator.sessionId).formatTime().getBuffer()
                            , listParticipants.getSize() );

            NEDirectConnection::MapParticipants & mapParticpants = getInitiatedConnections( );
            NEDirectConnection::ListParticipants listRegistered( mapParticpants.getAt( initiator ) );
            for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
            {
                const NEDirectConnection::sParticipant & particpant = listParticipants[i];
                for (uint32_t j = 0; j < listRegistered.getSize(); ++ j )
                {
                    if ( particpant == listRegistered[j] )
                    {
                        listRegistered.removeAt(j);
                        break;
                    }
                }
            }
            if ( listRegistered.isEmpty() )
            {
                mapParticpants.removeAt(initiator);
            }
            else
            {
                mapParticpants.setAt(initiator, listRegistered);
            }

            responseRemoveParticipant( true, listRegistered );
            NEDirectConnection::sInitiator      * wParam = new NEDirectConnection::sInitiator(initiator);
            NEDirectConnection::ListParticipants* lParam = new NEDirectConnection::ListParticipants(listRegistered);
            DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdDirectConnectionRemove, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
        }
        else
        {
            LOG_WARN("[ %s ] at time-stamps [ %s ] has no participants. Ignoring request to remove participants."
                            , initiator.nickName.getBuffer()
                            , DateTime(initiator.sessionId).formatTime().getBuffer() );
            responseRemoveParticipant(false, NEDirectConnection::ListParticipants());
        }
    }
    else
    {
        LOG_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]", initiator.nickName.getBuffer(), initiator.cookie, initiator.sessionId);
        responseRemoveParticipant( false, NEDirectConnection::ListParticipants( ) );
    }
}

void DirectConnectionService::requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestCloseConnection );
    NEDirectConnection::MapParticipants & mapParticipants = getInitiatedConnections( );
    mapParticipants.removeAt(initiator);
    NEDirectConnection::sInitiator      * wParam = new NEDirectConnection::sInitiator( initiator );
    NEDirectConnection::ListParticipants* lParam = nullptr;
    DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdDirectConnectionClose, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
}
