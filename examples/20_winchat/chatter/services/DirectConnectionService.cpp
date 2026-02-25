
/************************************************************************
 * \file            chatter/services/DirectConnectionService.cpp
 * \brief           The messaging service client object
 ************************************************************************/
#include "chatter/res/stdafx.h"
#include "chatter/services/DirectConnectionService.hpp"
#include "chatter/ui/DistributedDialog.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "areg/logging/GELog.h"
#include "areg/component/ComponentLoader.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

DEF_LOG_SCOPE( chatter_DirectConnectionService_requestConnectoinSetup );
DEF_LOG_SCOPE( chatter_DirectConnectionService_requestAddParticipant );
DEF_LOG_SCOPE( chatter_DirectConnectionService_requestRemoveParticipant );
DEF_LOG_SCOPE( chatter_DirectConnectionService_requestCloseConnection );

DirectConnectionService * DirectConnectionService::mService = nullptr;

DirectConnectionService * DirectConnectionService::GetService()
{
    return DirectConnectionService::mService;
}

areg::String DirectConnectionService::GetGeneratedService( const areg::String & nickName, uint32_t cookie )
{
    return NEDistributedApp::getConnectionServiceRole(nickName, cookie);
}

areg::Model DirectConnectionService::GetModel( const areg::String & nickName, uint32_t cookie, std::any data )
{
    areg::String    roleName    = DirectConnectionService::GetGeneratedService(nickName, cookie);
    areg::String    threadName  = NEDistributedApp::PREFIX_TRHEAD   + roleName;
    areg::String    modelName   = NEDistributedApp::PREFIX_MODEL    + roleName;

    areg::ServiceEntry          serviceEntry( DirectConnection::ServiceName, DirectConnection::InterfaceVersion );
    areg::ServiceList           serviceList( serviceEntry );
    areg::ComponentEntry        componentEntry(threadName, roleName
                                                    , FUNC_CREATE_COMP(DirectConnectionService)
                                                    , FUNC_DELETE_COMP
                                                    , serviceList, areg::DependencyList(), areg::WorkerThreadList());
    componentEntry.setData(data);
    areg::ComponentList         componentList(componentEntry);
    areg::ComponentThreadEntry  threadEntry(threadName, componentList);
    areg::ComponentThreadList   threadList( threadEntry );
    areg::Model                 model(modelName, threadList);
    
    return model;
}

DirectConnectionService::DirectConnectionService( const areg::ComponentEntry & entry, ComponentThread & ownerThread )
    : Component             ( entry, ownerThread )
    , DirectConnectionStub  ( static_cast<Component &>(self()) )

    , mNickName             ( std::any_cast<PageConnections *>(entry.getData())->GetRegisteredName() )
    , mCookie               ( std::any_cast<PageConnections *>(entry.getData())->GetRegisteredCookie() )
{
    DirectConnectionService::mService = this;
}

DirectConnectionService::~DirectConnectionService( )
{
    DirectConnectionService::mService = nullptr;
}

inline bool DirectConnectionService::isInitiatorValid( const DirectConnection::sInitiator & initiator ) const
{
    return (    (initiator.nickName.isEmpty()   == false                            ) && 
                (initiator.cookie               != DirectConnection::InvalidCookie) &&
                (initiator.sessionId            != DirectConnection::InvalidSession) );
}

inline bool DirectConnectionService::exists( const DirectConnection::sInitiator & initiator ) const
{
    return (getInitiatedConnections().contains(initiator));
}

uint64_t DirectConnectionService::getSession( const DirectConnection::ListParticipants & listParticipants )
{
    uint64_t result = DirectConnection::InvalidSession;
    for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
    {
        const DirectConnection::Participant & participant = listParticipants[i];
        if ( (participant.nickName == mNickName) && (participant.cookie == mCookie) )
        {
            result = participant.sessionId;
            break;
        }
    }
    return result;
}

void DirectConnectionService::requestConnectoinSetup( const DirectConnection::sInitiator & initiator, const DirectConnection::ListParticipants & listParticipants )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestConnectoinSetup );

    ASSERT(mNickName.isEmpty() == false);
    ASSERT(mCookie != DirectConnection::InvalidCookie);

    DirectConnection::Participant participant;
    participant.nickName = mNickName;
    participant.cookie   = mCookie;
    participant.sessionId= DirectConnection::InvalidSession;

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == false )
        {
            LOG_DBG("[ %s ] at time-stamps [ %s ] initiated chat with [ %d ] clients. Setting up chat."
                            , initiator.nickName.getBuffer()
                            , areg::DateTime(initiator.sessionId).formatTime().getBuffer()
                            , listParticipants.getSize() );

            uint64_t sessionID = getSession(listParticipants);
            participant.sessionId= sessionID;

            if ( sessionID != DirectConnection::InvalidSession )
            {
                DirectConnection::MapParticipants & mapParticipants = getInitiatedConnections( );
                mapParticipants.setAt( initiator, listParticipants );
                responseConnectoinSetup( true, participant, initiator, listParticipants );
                DirectConnection::sInitiator      * wParam    = new DirectConnection::sInitiator(initiator);
                DirectConnection::ListParticipants * lParam   = new DirectConnection::ListParticipants(listParticipants);

                DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdSetDirectConnection, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
            }
            else
            {
                LOG_DBG("[ %s ] with cookie [ %u ] could not find session ID. Ignoring direct connection setup.", mNickName.getBuffer(), mCookie);
                responseConnectoinSetup( false, participant, initiator, DirectConnection::ListParticipants() );
            }
        }
        else
        {
            const DirectConnection::MapParticipants &  mapParticipants = getInitiatedConnections( );
            const DirectConnection::ListParticipants & tempList = mapParticipants.getAt(initiator);
            LOG_WARN("[ %s ] at time-stamps [ %s ] has already initiated chat with [ %d ] clients. Ignoring chat setup."
                            , participant.nickName.getString()
                            , areg::DateTime(initiator.sessionId).formatTime().getString()
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
        responseConnectoinSetup( false, participant, initiator, DirectConnection::ListParticipants( ) );
    }
}

void DirectConnectionService::requestAddParticipant( const DirectConnection::sInitiator & initiator, const DirectConnection::ListParticipants & listParticipants )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestAddParticipant );

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == true )
        {
            LOG_DBG("[ %s ] at time-stamps [ %s ] is adding chat participants of [ %d ] clients."
                            , initiator.nickName.getBuffer()
                            , areg::DateTime(initiator.sessionId).formatTime().getBuffer()
                            , listParticipants.getSize() );

            DirectConnection::MapParticipants & mapParticipants = getInitiatedConnections( );
            DirectConnection::ListParticipants listRegistered (mapParticipants.getAt( initiator ));
            listRegistered.append(listParticipants);
            mapParticipants[initiator] = listRegistered;
            responseAddParticipant( true, listRegistered );
            DirectConnection::sInitiator      * wParam = new DirectConnection::sInitiator(initiator);
            DirectConnection::ListParticipants* lParam = new DirectConnection::ListParticipants(listRegistered);
            DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdDirectConnectionAdd, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
        }
        else
        {
            LOG_WARN("[ %s ] at time-stamps [ %s ] has no participants. Ignoring request to add participants."
                            , initiator.nickName.getBuffer()
                            , areg::DateTime(initiator.sessionId).formatTime().getBuffer() );
            responseAddParticipant(false, DirectConnection::ListParticipants());
        }
    }
    else
    {
        LOG_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]", initiator.nickName.getString(), initiator.cookie, initiator.sessionId);
        responseAddParticipant( false, DirectConnection::ListParticipants( ) );
    }
}

void DirectConnectionService::requestRemoveParticipant( const DirectConnection::sInitiator & initiator, const DirectConnection::ListParticipants & listParticipants )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestRemoveParticipant );

    if ( isInitiatorValid(initiator) )
    {
        if ( exists(initiator) == true )
        {
            LOG_DBG("[ %s ] at time-stamps [ %s ] is removing chat participants of [ %d ] clients."
                            , initiator.nickName.getBuffer()
                            , areg::DateTime(initiator.sessionId).formatTime().getBuffer()
                            , listParticipants.getSize() );

            DirectConnection::MapParticipants & mapParticpants = getInitiatedConnections( );
            DirectConnection::ListParticipants listRegistered( mapParticpants.getAt( initiator ) );
            for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
            {
                const DirectConnection::Participant & particpant = listParticipants[i];
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
            DirectConnection::sInitiator      * wParam = new DirectConnection::sInitiator(initiator);
            DirectConnection::ListParticipants* lParam = new DirectConnection::ListParticipants(listRegistered);
            DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdDirectConnectionRemove, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
        }
        else
        {
            LOG_WARN("[ %s ] at time-stamps [ %s ] has no participants. Ignoring request to remove participants."
                            , initiator.nickName.getBuffer()
                            , areg::DateTime(initiator.sessionId).formatTime().getBuffer() );
            responseRemoveParticipant(false, DirectConnection::ListParticipants());
        }
    }
    else
    {
        LOG_WARN("Invalid participant with name [ %s ], cookie [ %u ] and session ID [ %llu ]", initiator.nickName.getBuffer(), initiator.cookie, initiator.sessionId);
        responseRemoveParticipant( false, DirectConnection::ListParticipants( ) );
    }
}

void DirectConnectionService::requestCloseConnection( const DirectConnection::sInitiator & initiator )
{
    LOG_SCOPE( chatter_DirectConnectionService_requestCloseConnection );
    DirectConnection::MapParticipants & mapParticipants = getInitiatedConnections( );
    mapParticipants.removeAt(initiator);
    DirectConnection::sInitiator      * wParam = new DirectConnection::sInitiator( initiator );
    DirectConnection::ListParticipants* lParam = nullptr;
    DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdDirectConnectionClose, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam) );
}
