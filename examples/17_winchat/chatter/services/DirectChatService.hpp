#pragma once
/************************************************************************
 * \file            chatter/services/DirectChatService.hpp
 * \brief           The direct chat service with direct connection clients
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/DirectMessagerStub.hpp"
#include "chatter/services/DirectMessagingClient.hpp"

#include "areg/component/NERegistry.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "generated/NEDirectConnection.hpp"
#include "generated/NECommon.hpp"
#include "chatter/NEDistributedApp.hpp"

class DirectConnectionClient;
class ChatPrticipantHandler;

class DirectChatService : public Component
                        , public DirectMessagerStub
                          
{
    using HashMapDirectConnections      = TEMap<String, DirectChatService *>;
    using MapDirectConnections          = TELockResourceMap<String, DirectChatService *, HashMapDirectConnections>;
    using ListDirectConnectionClients   = TEArrayList<DirectConnectionClient *>;

//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

    static NERegistry::Model GetModel( const NEDirectMessager::sParticipant & initiator, const NEDirectMessager::ListParticipants & listParticipants, const NEMemory::uAlign data );

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The time-stamp when the request was sent.
     * \see     responseChatJoin
     **/
    virtual void requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect );

    /**
     * \brief   Request call.
     *          Request to send a message, has no response
     * \param   sender      The participant structure of message sender
     * \param   msgText     The text message to send
     * \param   timeSent    The time-stamp when the message is requested to send.
     * \note    Has no response
     **/
    virtual void requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );

    /**
     * \brief   Request call.
     *          The request to send message while typing a text. Has no response.
     * \param   participant The structure of participant while sending message during typing.
     * \param   msgText     The text message while typing.
     * \note    Has no response
     **/
    virtual void requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText );

    /**
     * \brief   Request call.
     *          The request to leave chat-room. Has no response.
     * \param   participant The structure of chat-room participant.
     * \param   timeLeave   Time-stamp when it was requested to leave chat-room.
     * \note    Has no response
     **/
    virtual void requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave );

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
    virtual void startupComponent( ComponentThread & comThread );

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void shutdownComponent( ComponentThread & comThread );

/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    DirectChatService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread, ChatPrticipantHandler & handlerParticipant );
    virtual ~DirectChatService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    static MapDirectConnections _mapDirectConnections;

    ChatPrticipantHandler &     mPaticipantsHandler;
    ListDirectConnectionClients mListClients;
    DirectMessagingClient       mChatParticipant;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline DirectChatService & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    DirectChatService( void );
    DirectChatService( const DirectChatService & /*src*/ );
    const DirectChatService & operator = ( const DirectChatService & /*src*/ );
};

inline DirectChatService & DirectChatService::self( void )
{   return (*this); }
