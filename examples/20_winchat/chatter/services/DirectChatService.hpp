#pragma once
/************************************************************************
 * \file            chatter/services/DirectChatService.hpp
 * \brief           The direct chat service with direct connection clients
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/20_winchat/services/DirectMessagerProviderBase.hpp"
#include "chatter/services/DirectMessagingClient.hpp"

#include "areg/component/Model.hpp"
#include "areg/base/ResourceMap.hpp"
#include "examples/20_winchat/services/DirectConnection.hpp"
#include "common/ChatDefs.hpp"

class DirectConnectionClient;
class ChatPrticipantHandler;

class DirectChatService final : public areg::Component
                              , public DirectMessagerProviderBase
{
    using HashMapDirectConnections      = areg::OrderedMap<areg::String, DirectChatService *>;
    using MapDirectConnections          = areg::ConcurrentResourceMap<areg::String, DirectChatService *, HashMapDirectConnections>;
    using ListDirectConnectionClients   = areg::ArrayList<DirectConnectionClient *>;

//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static areg::Model GetModel( const DirectMessager::Participant & initiator, const DirectMessager::ListParticipants & listParticipants, std::any data );

public:
    DirectChatService( const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread );
    ~DirectChatService();

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The time-stamp when the request was sent.
     * \see     response_chat_join
     **/
    void request_chat_join( const DirectMessager::Participant & participant, const areg::DateTime & timeConnect ) final;

    /**
     * \brief   Request call.
     *          Request to send a message, has no response
     * \param   sender      The participant structure of message sender
     * \param   msgText     The text message to send
     * \param   timeSent    The time-stamp when the message is requested to send.
     * \note    Has no response
     **/
    void request_message_send( const DirectMessager::Participant & sender, const areg::String & msgText, const areg::DateTime & timeSent ) final;

    /**
     * \brief   Request call.
     *          The request to send message while typing a text. Has no response.
     * \param   participant The structure of participant while sending message during typing.
     * \param   msgText     The text message while typing.
     * \note    Has no response
     **/
    void request_message_type( const DirectMessager::Participant & participant, const areg::String & msgText ) final;

    /**
     * \brief   Request call.
     *          The request to leave chat-room. Has no response.
     * \param   participant The structure of chat-room participant.
     * \param   timeLeave   Time-stamp when it was requested to leave chat-room.
     * \note    Has no response
     **/
    void request_chat_leave( const DirectMessager::Participant & participant, const areg::DateTime & timeLeave ) final;

protected:

/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    void startup_component( areg::ComponentThread & comThread ) final;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    void shutdown_component( areg::ComponentThread & comThread ) final;

/************************************************************************/
// ProviderBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Provider,
     *                  which started up.
     **/
    void startup_service_interface( areg::Component & holder ) final;
    
//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline void _clearList();
    
//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    static MapDirectConnections _mapDirectConnections;

    ChatPrticipantHandler *     mPaticipantsHandler;
    ListDirectConnectionClients mListClients;
    DirectMessagingClient       mChatParticipant;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline DirectChatService & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    DirectChatService();
    DirectChatService( const DirectChatService & /*src*/ );
    const DirectChatService & operator = ( const DirectChatService & /*src*/ );
};

inline DirectChatService & DirectChatService::self()
{   return (*this); }
