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

DirectConnectionClient::~DirectConnectionClient( )
{
}

bool DirectConnectionClient::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    bool result = false;
    if ( (isConnected == true) && DirectConnectionClientBase::serviceConnected( isConnected, proxy ) )
    {
        requestConnectoinSetup( mParticipantsHandler.GetInitiator( ), mParticipantsHandler.GetParticipantList() );
        result = true;
    }
    else if ( (isConnected == false) && DirectConnectionClientBase::serviceConnected( isConnected, proxy ) )
    {
        requestCloseConnection(mParticipantsHandler.GetInitiator());
        result = true;
    }
    return result;
}

void DirectConnectionClient::responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    ASSERT(mParticipantsHandler.GetInitiator() == initiator);
}
