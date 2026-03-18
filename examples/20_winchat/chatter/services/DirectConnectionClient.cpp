/************************************************************************
 * \file            chatter/services/DirectConnectionClient.cpp
 * \brief           The direct connection service client object
 ************************************************************************/

#include "chatter/services/DirectConnectionClient.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"


 DirectConnectionClient::DirectConnectionClient( areg::Component & owner, ChatPrticipantHandler * participantsHandler, const DirectConnection::Participant & target )
    : DirectConnectionConsumerBase  ( NEDistributedApp::getConnectionServiceRole(target.nickName, target.cookie).as_string(), owner )

    , mParticipantsHandler          ( participantsHandler )
{
     ASSERT(mParticipantsHandler != nullptr);
}

bool DirectConnectionClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    bool result = DirectConnectionConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        requestConnectoinSetup( mParticipantsHandler->GetInitiator( ), mParticipantsHandler->GetParticipantList( ) );
    }
    else
    {
        requestCloseConnection( mParticipantsHandler->GetInitiator( ) );
    }

    return result;
}

#ifdef  DEBUG
void DirectConnectionClient::responseConnectoinSetup( bool /* succeeded */
                                                    , const DirectConnection::Participant & /* target */
                                                    , const DirectConnection::sInitiator & initiator
                                                    , const DirectConnection::ListParticipants & /* listParticipants */ )
{
    ASSERT(mParticipantsHandler->GetInitiator() == initiator);
}
#else   // DEBUG
void DirectConnectionClient::responseConnectoinSetup( bool /* succeeded */
                                                    , const DirectConnection::Participant & /* target */
                                                    , const DirectConnection::sInitiator & /*initiator*/
                                                    , const DirectConnection::ListParticipants & /* listParticipants */ )
{
}
#endif  // DEBUG
