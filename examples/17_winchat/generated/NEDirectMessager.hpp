//////////////////////////////////////////////////////////////////////////
// Begin generate generated/NEDirectMessager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_NEDIRECTMESSAGER_HPP
#define  GENERATED_NEDIRECTMESSAGER_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     04.07.2021  04:30:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/NEDirectMessager.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Namespace declaration.
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
#include "areg/component/NEService.hpp"

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
     * \brief   Service Interface Name
     *          NEDirectMessager::ServiceName
     **/
    const char * const        ServiceName         = "DirectMessager";
    /**
     * \brief   Implemented Version
     *          NEDirectMessager::InterfaceVersion
     **/
    const Version             InterfaceVersion    = "1.0.0";

    /**
     * \brief   The maximum length of a message.
     *          NEDirectMessager::MessageMaxLen
     **/
    const unsigned int        MessageMaxLen       = NECommon::MaxlenMessage;

    /**
     * \brief   Identifies invalid session value.
     *          NEDirectMessager::InvalidSession
     **/
    const unsigned long long  InvalidSession      = NECommon::InvalidSession;

    /**
     * \brief   Identifies the invalid cookie.
     *          NEDirectMessager::InvalidCookie
     **/
    const unsigned int        InvalidCookie       = NECommon::InvalidCookie;

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
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestChatJoin
         *          Request to join chat. The participant should be in the list of connections
         **/
          MSG_ID_requestChatJoin              = NEService::REQUEST_ID_FIRST     // void requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect );
        /**
         * \brief   Request ID: MSG_ID_requestMessageSend
         *          Request to send a message, has no response
         **/
        , MSG_ID_requestMessageSend                                             // void requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );
        /**
         * \brief   Request ID: MSG_ID_requestMessageType
         *          The request to send message while typing a text. Has no response.
         **/
        , MSG_ID_requestMessageType                                             // void requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText );
        /**
         * \brief   Request ID: MSG_ID_requestChatLeave
         *          The request to leave chat-room. Has no response.
         **/
        , MSG_ID_requestChatLeave                                               // void requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_responseChatJoin
         *          Response to join chat
         **/
        , MSG_ID_responseChatJoin             = NEService::RESPONSE_ID_FIRST    // void responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastMessageSent
         *          Informs all connected servicing clients that the message is sent.
         **/
        , MSG_ID_broadcastMessageSent                                           // void broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastMessageTyped
         *          Informs all connected servicing client that the text was typed.
         **/
        , MSG_ID_broadcastMessageTyped                                          // void broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastParticipantJoined
         *          Informs new participant joined chat-room
         **/
        , MSG_ID_broadcastParticipantJoined                                     // void broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastParticipantLeft
         *          Informs that a participant left chat-room.
         **/
        , MSG_ID_broadcastParticipantLeft                                       // void broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastChatClosed
         *          Informs all service connected clients that the chat-room is closed. Message will be impossible.
         **/
        , MSG_ID_broadcastChatClosed                                            // void broadcastChatClosed( void );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_ChatParticipants
         *          The list of char-room participants
         **/
        , MSG_ID_ChatParticipants             = NEService::ATTRIBUTE_ID_FIRST   // NEDirectMessager::ListParticipants mChatParticipants;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED                   = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID                      = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
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
    case    NEDirectMessager::MSG_ID_requestChatJoin:
        return "NEDirectMessager::MSG_ID_requestChatJoin";
    case    NEDirectMessager::MSG_ID_requestMessageSend:
        return "NEDirectMessager::MSG_ID_requestMessageSend";
    case    NEDirectMessager::MSG_ID_requestMessageType:
        return "NEDirectMessager::MSG_ID_requestMessageType";
    case    NEDirectMessager::MSG_ID_requestChatLeave:
        return "NEDirectMessager::MSG_ID_requestChatLeave";
    case    NEDirectMessager::MSG_ID_responseChatJoin:
        return "NEDirectMessager::MSG_ID_responseChatJoin";
    case    NEDirectMessager::MSG_ID_broadcastMessageSent:
        return "NEDirectMessager::MSG_ID_broadcastMessageSent";
    case    NEDirectMessager::MSG_ID_broadcastMessageTyped:
        return "NEDirectMessager::MSG_ID_broadcastMessageTyped";
    case    NEDirectMessager::MSG_ID_broadcastParticipantJoined:
        return "NEDirectMessager::MSG_ID_broadcastParticipantJoined";
    case    NEDirectMessager::MSG_ID_broadcastParticipantLeft:
        return "NEDirectMessager::MSG_ID_broadcastParticipantLeft";
    case    NEDirectMessager::MSG_ID_broadcastChatClosed:
        return "NEDirectMessager::MSG_ID_broadcastChatClosed";
    case    NEDirectMessager::MSG_ID_ChatParticipants:
        return "NEDirectMessager::MSG_ID_ChatParticipants";

    case NEDirectMessager::MSG_ID_NO_PROCEED:
        return "NEDirectMessager::MSG_ID_NO_PROCEED";
        
    case NEDirectMessager::MSG_ID_INVALID:
        return "NEDirectMessager::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEDirectMessager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEDirectMessager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_NEDIRECTMESSAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/NEDirectMessager.hpp file
//////////////////////////////////////////////////////////////////////////
