#pragma once
/************************************************************************
 * \file            chatter/services/DirectConnectionClient.hpp
 * \brief           The direct connection service client object
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/src/DirectConnectionClientBase.hpp"
#include "areg/component/NERegistry.hpp"

class ChatPrticipantHandler;

class DirectConnectionClient  : public DirectConnectionClientBase
{
public:
    DirectConnectionClient( Component & owner, ChatPrticipantHandler & participantsHandler, const NEDirectConnection::sParticipant & target );
    virtual ~DirectConnectionClient( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Response callback.
     *          The response of connection setup
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeeded           Flag, indicating whether the connection setup succeeded or not.
     * \param   target              The targeted participant to include in chat-room
     * \param   initiator           The chat-room initiator
     * \param   listParticipants    List of chat-room participants
     * \see     requestConnectoinSetup
     **/
    virtual void responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );

/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    virtual bool serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    ChatPrticipantHandler &       mParticipantsHandler;

private:
    DirectConnectionClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectConnectionClient );
};
