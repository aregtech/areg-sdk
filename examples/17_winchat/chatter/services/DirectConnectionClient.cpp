/************************************************************************
 * \file            chatter/services/DirectConnectionClient.cpp
 * \brief           The direct connection service client object
 ************************************************************************/

#include "chatter/services/DirectConnectionClient.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"


 DirectConnectionClient::DirectConnectionClient( Component & owner, ChatPrticipantHandler & participantsHandler, const NEDirectConnection::sParticipant & target )
    : DirectConnectionClientBase  ( NEDistributedApp::getConnectionServiceRole(target.nickName, target.cookie).getString(), owner )

    , mParticipantsHandler          ( participantsHandler )
{
}

bool DirectConnectionClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    bool result = DirectConnectionClientBase::serviceConnected( status, proxy );
    if ( isConnected( ) )
    {
        requestConnectoinSetup( mParticipantsHandler.GetInitiator( ), mParticipantsHandler.GetParticipantList( ) );
    }
    else
    {
        requestCloseConnection( mParticipantsHandler.GetInitiator( ) );
    }

    return result;
}

void DirectConnectionClient::responseConnectoinSetup( bool /* succeeded */
                                                    , const NEDirectConnection::sParticipant & /* target */
                                                    , const NEDirectConnection::sInitiator & initiator
                                                    , const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    ASSERT(mParticipantsHandler.GetInitiator() == initiator);
}
