//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/NECentralMessager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_NECENTRALMESSAGER_HPP
#define  SHARED_GENERATED_NECENTRALMESSAGER_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 * Generated at     04.07.2021  04:29:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/NECentralMessager.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "shared/NECommonSettings.hpp"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

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
     * \brief   Service Interface Name
     *          NECentralMessager::ServiceName
     **/
    const char * const    ServiceName         = "CentralMessager";
    /**
     * \brief   Implemented Version
     *          NECentralMessager::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   The maximum length of messages.
     *          NECentralMessager::MessageMaxLen
     **/
    const unsigned int    MessageMaxLen       = NECommonSettings::MaxlenMessage;

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
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestSendMessage
         *          Request to sends message that all clients can see.
         **/
          MSG_ID_requestSendMessage           = NEService::REQUEST_ID_FIRST     // void requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );
        /**
         * \brief   Request ID: MSG_ID_requestKeyTyping
         *          Sent each time when typing a key
         **/
        , MSG_ID_requestKeyTyping                                               // void requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastSendMessage
         *          The response, sent by connection manager to notify the message typing update
         **/
        , MSG_ID_broadcastSendMessage         = NEService::RESPONSE_ID_FIRST    // void broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastKeyTyping
         *          Sent each time when a client is typing a key
         **/
        , MSG_ID_broadcastKeyTyping                                             // void broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastBroadcastMessage
         *          Server broadcasts a message to all clients.
         **/
        , MSG_ID_broadcastBroadcastMessage                                      // void broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
     
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
    case    NECentralMessager::MSG_ID_requestSendMessage:
        return "NECentralMessager::MSG_ID_requestSendMessage";
    case    NECentralMessager::MSG_ID_requestKeyTyping:
        return "NECentralMessager::MSG_ID_requestKeyTyping";
    case    NECentralMessager::MSG_ID_broadcastSendMessage:
        return "NECentralMessager::MSG_ID_broadcastSendMessage";
    case    NECentralMessager::MSG_ID_broadcastKeyTyping:
        return "NECentralMessager::MSG_ID_broadcastKeyTyping";
    case    NECentralMessager::MSG_ID_broadcastBroadcastMessage:
        return "NECentralMessager::MSG_ID_broadcastBroadcastMessage";

    case NECentralMessager::MSG_ID_NO_PROCEED:
        return "NECentralMessager::MSG_ID_NO_PROCEED";
        
    case NECentralMessager::MSG_ID_INVALID:
        return "NECentralMessager::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NECentralMessager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NECentralMessager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHARED_GENERATED_NECENTRALMESSAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/NECentralMessager.hpp file
//////////////////////////////////////////////////////////////////////////
