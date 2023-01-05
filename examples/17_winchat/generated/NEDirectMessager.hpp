//////////////////////////////////////////////////////////////////////////
// Begin generate generated/NEDirectMessager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_NEDIRECTMESSAGER_HPP
#define  GENERATED_NEDIRECTMESSAGER_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/NEDirectMessager.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "generated/NECommon.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg//component/NERegistry.hpp"
#include "areg/component/NEService.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   DirectMessager Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          The service interface of direct connection and messaging used in chat-room.
 *
 **/
namespace   NEDirectMessager
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEDirectMessager::ServiceName
     **/
    constexpr   char    ServiceName[]   { "DirectMessager" };
    /**
     * \brief   Implemented Version.
     *          NEDirectMessager::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The maximum length of a message.
     *          NEDirectMessager::MessageMaxLen
     **/
    constexpr unsigned int        MessageMaxLen       = NECommon::MaxlenMessage;

    /**
     * \brief   Identifies invalid session value.
     *          NEDirectMessager::InvalidSession
     **/
    constexpr unsigned long long  InvalidSession      = NECommon::InvalidSession;

    /**
     * \brief   Identifies the invalid cookie.
     *          NEDirectMessager::InvalidCookie
     **/
    constexpr unsigned int        InvalidCookie       = NECommon::InvalidCookie;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The imported structure of participant
     *          NEDirectMessager::sParticipant
     **/
     typedef    NECommon::sParticipant         sParticipant;

    /**
     * \brief   Imported list of participants.
     *          NEDirectMessager::ListParticipants
     **/
     typedef    NECommon::ListParticipants     ListParticipants;

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEDirectMessager::eMessageIDs
     *          DirectMessager Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestChatJoin
         *          Request to join chat. The participant should be in the list of connections
         **/
          MsgId_requestChatJoin               = NEService::REQUEST_ID_FIRST   // void requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect );
        /**
         * \brief   Request ID: MsgId_requestMessageSend
         *          Request to send a message, has no response
         **/
        , MsgId_requestMessageSend                                              // void requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );
        /**
         * \brief   Request ID: MsgId_requestMessageType
         *          The request to send message while typing a text. Has no response.
         **/
        , MsgId_requestMessageType                                              // void requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText );
        /**
         * \brief   Request ID: MsgId_requestChatLeave
         *          The request to leave chat-room. Has no response.
         **/
        , MsgId_requestChatLeave                                                // void requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseChatJoin
         *          Response to join chat
         **/
        , MsgId_responseChatJoin              = NEService::RESPONSE_ID_FIRST  // void responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MsgId_broadcastMessageSent
         *          Informs all connected servicing clients that the message is sent.
         **/
        , MsgId_broadcastMessageSent                                            // void broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );
        /**
         * \brief   Broadcast ID: MsgId_broadcastMessageTyped
         *          Informs all connected servicing client that the text was typed.
         **/
        , MsgId_broadcastMessageTyped                                           // void broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText );
        /**
         * \brief   Broadcast ID: MsgId_broadcastParticipantJoined
         *          Informs new participant joined chat-room
         **/
        , MsgId_broadcastParticipantJoined                                      // void broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined );
        /**
         * \brief   Broadcast ID: MsgId_broadcastParticipantLeft
         *          Informs that a participant left chat-room.
         **/
        , MsgId_broadcastParticipantLeft                                        // void broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft );
        /**
         * \brief   Broadcast ID: MsgId_broadcastChatClosed
         *          Informs all service connected clients that the chat-room is closed. Message will be impossible.
         **/
        , MsgId_broadcastChatClosed                                             // void broadcastChatClosed( void );

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_ChatParticipants
         *          The list of char-room participants
         **/
        , MsgId_ChatParticipants              = NEService::ATTRIBUTE_ID_FIRST // NEDirectMessager::ListParticipants mChatParticipants;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed                  = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                       = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NEDirectMessager::eMessageIDs
     **/
    inline const char * getString( NEDirectMessager::eMessageIDs msgId );

/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/

    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & getInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEDirectMessager::eMessageIDs getResponseId( NEDirectMessager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEDirectMessager::eMessageIDs getRequestId( NEDirectMessager::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEDirectMessager methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEDirectMessager::eMessageIDs );

/**
 * Returns string value of NEDirectMessager::eMessageIDs
 **/
inline const char * NEDirectMessager::getString( NEDirectMessager::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEDirectMessager::eMessageIDs::MsgId_requestChatJoin:
        return "NEDirectMessager::eMessageIDs::MsgId_requestChatJoin";
    case    NEDirectMessager::eMessageIDs::MsgId_requestMessageSend:
        return "NEDirectMessager::eMessageIDs::MsgId_requestMessageSend";
    case    NEDirectMessager::eMessageIDs::MsgId_requestMessageType:
        return "NEDirectMessager::eMessageIDs::MsgId_requestMessageType";
    case    NEDirectMessager::eMessageIDs::MsgId_requestChatLeave:
        return "NEDirectMessager::eMessageIDs::MsgId_requestChatLeave";
    case    NEDirectMessager::eMessageIDs::MsgId_responseChatJoin:
        return "NEDirectMessager::eMessageIDs::MsgId_responseChatJoin";
    case    NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent:
        return "NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent";
    case    NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped:
        return "NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped";
    case    NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined:
        return "NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined";
    case    NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft:
        return "NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft";
    case    NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed:
        return "NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed";
    case    NEDirectMessager::eMessageIDs::MsgId_ChatParticipants:
        return "NEDirectMessager::eMessageIDs::MsgId_ChatParticipants";

    case NEDirectMessager::eMessageIDs::MsgId_NotProcessed:
        return "NEDirectMessager::eMessageIDs::MsgId_NotProcessed";
    case NEDirectMessager::eMessageIDs::MsgId_Invalid:
        return "NEDirectMessager::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEDirectMessager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_NEDIRECTMESSAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/NEDirectMessager.hpp file
//////////////////////////////////////////////////////////////////////////
