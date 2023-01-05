//////////////////////////////////////////////////////////////////////////
// Begin generate generated/NECentralMessager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_NECENTRALMESSAGER_HPP
#define  GENERATED_NECENTRALMESSAGER_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/NECentralMessager.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
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
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

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
// NECentralMessager namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   CentralMessager Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          This service is used to receive messages from connected edge when typing or 
 *          sending messages. The service receives text message from initiator and
 *          broadcast to all clients, which have subscribed on event.
 *          It as well broadcasts own text message to all connected clients.
 *
 **/
namespace   NECentralMessager
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NECentralMessager::ServiceName
     **/
    constexpr   char    ServiceName[]   { "CentralMessager" };
    /**
     * \brief   Implemented Version.
     *          NECentralMessager::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The maximum length of messages.
     *          NECentralMessager::MessageMaxLen
     **/
    constexpr unsigned int    MessageMaxLen       = NECommon::MaxlenMessage;

/************************************************************************/
// Service Interface data types
/************************************************************************/


/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NECentralMessager::eMessageIDs
     *          CentralMessager Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestSendMessage
         *          Request to sends message that all clients can see.
         **/
          MsgId_requestSendMessage        = NEService::REQUEST_ID_FIRST   // void requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );
        /**
         * \brief   Request ID: MsgId_requestKeyTyping
         *          Sent each time when typing a key
         **/
        , MsgId_requestKeyTyping                                            // void requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );

    /************************************************************************
     * Response IDs
     ************************************************************************/
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MsgId_broadcastSendMessage
         *          The response, sent by connection manager to notify the message typing update
         **/
        , MsgId_broadcastSendMessage      = NEService::RESPONSE_ID_FIRST  // void broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );
        /**
         * \brief   Broadcast ID: MsgId_broadcastKeyTyping
         *          Sent each time when a client is typing a key
         **/
        , MsgId_broadcastKeyTyping                                          // void broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );
        /**
         * \brief   Broadcast ID: MsgId_broadcastBroadcastMessage
         *          Server broadcasts a message to all clients.
         **/
        , MsgId_broadcastBroadcastMessage                                   // void broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime );

    /************************************************************************
     * Attributes IDs
     ************************************************************************/

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed              = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                   = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NECentralMessager::eMessageIDs
     **/
    inline const char * getString( NECentralMessager::eMessageIDs msgId );

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
    NECentralMessager::eMessageIDs getResponseId( NECentralMessager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NECentralMessager::eMessageIDs getRequestId( NECentralMessager::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NECentralMessager inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NECentralMessager methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NECentralMessager::eMessageIDs );

/**
 * Returns string value of NECentralMessager::eMessageIDs
 **/
inline const char * NECentralMessager::getString( NECentralMessager::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NECentralMessager::eMessageIDs::MsgId_requestSendMessage:
        return "NECentralMessager::eMessageIDs::MsgId_requestSendMessage";
    case    NECentralMessager::eMessageIDs::MsgId_requestKeyTyping:
        return "NECentralMessager::eMessageIDs::MsgId_requestKeyTyping";
    case    NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage:
        return "NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage";
    case    NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping:
        return "NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping";
    case    NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage:
        return "NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage";

    case NECentralMessager::eMessageIDs::MsgId_NotProcessed:
        return "NECentralMessager::eMessageIDs::MsgId_NotProcessed";
    case NECentralMessager::eMessageIDs::MsgId_Invalid:
        return "NECentralMessager::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NECentralMessager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NECentralMessager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_NECENTRALMESSAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/NECentralMessager.hpp file
//////////////////////////////////////////////////////////////////////////
