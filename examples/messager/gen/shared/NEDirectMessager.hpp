//////////////////////////////////////////////////////////////////////////
// Begin generate gen/shared/NEDirectMessager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GEN_SHARED_NEDIRECTMESSAGER_HPP
#define  GEN_SHARED_NEDIRECTMESSAGER_HPP

/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     24.08.2019  21:12:33 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/shared/NEDirectMessager.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "distrbutedapp/NEDistributedData.hpp"
#include "areg/core/base/CEDateTime.hpp"
#include "areg/core/base/CEString.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/core/base/IEIOStream.hpp"
#include "areg/core/base/CEVersion.hpp"
#include "areg/core/component/NEService.hpp"

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
    const CEVersion           InterfaceVersion    = "1.0.0";

    /**
     * \brief   The maximum length of a message.
     *          NEDirectMessager::MessageMaxLen
     **/
    const unsigned int        MessageMaxLen       = NEDistributedData::MaxlenMessage;

    /**
     * \brief   Identifies invalid session value.
     *          NEDirectMessager::InvalidSession
     **/
    const unsigned long long  InvalidSession      = NEDistributedData::InvalidSession;

    /**
     * \brief   Identifies the invalid cookie.
     *          NEDirectMessager::InvalidCookie
     **/
    const unsigned int        InvalidCookie       = NEDistributedData::InvalidCookie;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The imported structure of participant
     *          NEDirectMessager::sParticipant
     **/
     typedef    NEDistributedData::sParticipant        sParticipant;

    /**
     * \brief   Imported list of participants.
     *          NEDirectMessager::ListParticipants
     **/
     typedef    NEDistributedData::ListParticipants    ListParticipants;

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
         * \brief   Request ID: MSG_ID_RequestChatJoin
         *          Request to join chat. The participant should be in the list of connections
         **/
          MSG_ID_RequestChatJoin              = NEService::REQUEST_ID_FIRST     // void RequestChatJoin( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect );
        /**
         * \brief   Request ID: MSG_ID_RequestMessageSend
         *          Request to send a message, has no response
         **/
        , MSG_ID_RequestMessageSend                                             // void RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );
        /**
         * \brief   Request ID: MSG_ID_RequestMessageType
         *          The request to send message while typing a text. Has no response.
         **/
        , MSG_ID_RequestMessageType                                             // void RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText );
        /**
         * \brief   Request ID: MSG_ID_RequestChatLeave
         *          The request to leave chat-room. Has no response.
         **/
        , MSG_ID_RequestChatLeave                                               // void RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_ResponseChatJoin
         *          Response to join chat
         **/
        , MSG_ID_ResponseChatJoin             = NEService::RESPONSE_ID_FIRST    // void ResponseChatJoin( const bool & succeed, const NEDirectMessager::ListParticipants & listParticipant, const CEDateTime & timeConnect, const CEDateTime & timeConnected );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastMessageSent
         *          Informs all connected servicing clients that the message is sent.
         **/
        , MSG_ID_BroadcastMessageSent                                           // void BroadcastMessageSent( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastMessageTyped
         *          Informs all connected servicing client that the text was typed.
         **/
        , MSG_ID_BroadcastMessageTyped                                          // void BroadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const CEString & msgText );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastParticipantJoined
         *          Informs new participant joined chat-room
         **/
        , MSG_ID_BroadcastParticipantJoined                                     // void BroadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeJoined );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastParticipantLeft
         *          Informes that a participant left chat-room.
         **/
        , MSG_ID_BroadcastParticipantLeft                                       // void BroadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeft );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastChatClosed
         *          Informes all service connected clients that the chat-room is closed. Message will be impossible.
         **/
        , MSG_ID_BroadcastChatClosed                                            // void BroadcastChatClosed( void );
     
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
    inline const char * const GetString( const NEDirectMessager::eMessageIDs msgId );
     
/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/
 
    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & CreateInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEDirectMessager::eMessageIDs GetResponseId( const NEDirectMessager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEDirectMessager::eMessageIDs GetRequestId( const NEDirectMessager::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_RequestChatJoin
        , MSG_ID_RequestMessageSend
        , MSG_ID_RequestMessageType
        , MSG_ID_RequestChatLeave
    };
    /**
     * \brief   Number of supported Request entries
     **/
    const unsigned int NumberofRequests     = MACRO_ARRAYLEN(RequestIds);

    /**
     * \brief   Array of supported Response IDs
     **/
    const eMessageIDs    ResponseIds[] = 
    {
          MSG_ID_ResponseChatJoin

        , MSG_ID_BroadcastMessageSent
        , MSG_ID_BroadcastMessageTyped
        , MSG_ID_BroadcastParticipantJoined
        , MSG_ID_BroadcastParticipantLeft
        , MSG_ID_BroadcastChatClosed
    };
    /**
     * \brief   Number of supported Response entries
     **/
    const unsigned int  NumberofResponses   = MACRO_ARRAYLEN(ResponseIds);

    /**
     * \brief   Array of supported Attribute IDs
     **/
    const eMessageIDs    AttributeIds[] =
    {
          MSG_ID_ChatParticipants
    };
    /**
     * \brief   Number of supported Attribute entries
     **/
    const unsigned int NumberofAttributes   = MACRO_ARRAYLEN(AttributeIds);

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
inline const char * const NEDirectMessager::GetString( const NEDirectMessager::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEDirectMessager::MSG_ID_RequestChatJoin:
        return "NEDirectMessager::MSG_ID_RequestChatJoin";
    case    NEDirectMessager::MSG_ID_RequestMessageSend:
        return "NEDirectMessager::MSG_ID_RequestMessageSend";
    case    NEDirectMessager::MSG_ID_RequestMessageType:
        return "NEDirectMessager::MSG_ID_RequestMessageType";
    case    NEDirectMessager::MSG_ID_RequestChatLeave:
        return "NEDirectMessager::MSG_ID_RequestChatLeave";
    case    NEDirectMessager::MSG_ID_ResponseChatJoin:
        return "NEDirectMessager::MSG_ID_ResponseChatJoin";
    case    NEDirectMessager::MSG_ID_BroadcastMessageSent:
        return "NEDirectMessager::MSG_ID_BroadcastMessageSent";
    case    NEDirectMessager::MSG_ID_BroadcastMessageTyped:
        return "NEDirectMessager::MSG_ID_BroadcastMessageTyped";
    case    NEDirectMessager::MSG_ID_BroadcastParticipantJoined:
        return "NEDirectMessager::MSG_ID_BroadcastParticipantJoined";
    case    NEDirectMessager::MSG_ID_BroadcastParticipantLeft:
        return "NEDirectMessager::MSG_ID_BroadcastParticipantLeft";
    case    NEDirectMessager::MSG_ID_BroadcastChatClosed:
        return "NEDirectMessager::MSG_ID_BroadcastChatClosed";
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

#endif   // GEN_SHARED_NEDIRECTMESSAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate gen/shared/NEDirectMessager.hpp file
//////////////////////////////////////////////////////////////////////////
