/************************************************************************
 * \file            edge/services/CEDirectConnectionClient.cpp
 * \brief           The direct connection service client object
 ************************************************************************/

#include "edge/services/CEDirectConnectionClient.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/services/CEChatPrticipantHandler.hpp"


 CEDirectConnectionClient::CEDirectConnectionClient( CEComponent & owner, CEChatPrticipantHandler & participantsHandler, const NEDirectConnection::sParticipant & target )
    : CEDirectConnectionClientBase  ( NEDistributedApp::getConnectionServiceRole(target.nickName, target.cookie).String(), owner )

    , mParticipantsHandler          ( participantsHandler )
{
}

CEDirectConnectionClient::~CEDirectConnectionClient( )
{
}

bool CEDirectConnectionClient::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    bool result = false;
    if ( (isConnected == true) && CEDirectConnectionClientBase::ServiceConnected( isConnected, proxy ) )
    {
        RequestConnectoinSetup( mParticipantsHandler.GetInitiator( ), mParticipantsHandler.GetParticipantList() );
        result = true;
    }
    else if ( (isConnected == false) && CEDirectConnectionClientBase::ServiceConnected( isConnected, proxy ) )
    {
        RequestCloseConnection(mParticipantsHandler.GetInitiator());
        result = true;
    }
    return result;
}

void CEDirectConnectionClient::ResponseConnectoinSetup( const bool & succeeded
                                                      , const NEDirectConnection::sParticipant & target
                                                      , const NEDirectConnection::sInitiator & initiator
                                                      , const NEDirectConnection::ListParticipants & listParticipants)
{
    ASSERT(mParticipantsHandler.GetInitiator() == initiator);
}
