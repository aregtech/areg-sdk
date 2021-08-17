#pragma once
/************************************************************************
 * \file            chatter/services/DirectConnectionClient.hpp
 * \brief           The direct connection service client object
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/DirectConnectionClientBase.hpp"
#include "areg/component/NERegistry.hpp"

class ChatPrticipantHandler;

class DirectConnectionClient  : public DirectConnectionClientBase
{
public:

public:
    DirectConnectionClient( Component & owner, ChatPrticipantHandler & participantsHandler, const NEDirectConnection::sParticipant & target );
    virtual ~DirectConnectionClient( void );

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

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

private:
    ChatPrticipantHandler &       mParticipantsHandler;

private:
    DirectConnectionClient( void );
    DirectConnectionClient( const DirectConnectionClient & /*src*/ );
    const DirectConnectionClient & operator = ( const DirectConnectionClient & /*src*/ );
};
