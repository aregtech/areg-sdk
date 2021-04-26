//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/NECentralMessager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_NECENTRALMESSAGER_HPP
#define  SHARED_GENERATED_NECENTRALMESSAGER_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     25.04.2021  23:50:42 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/NECentralMessager.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
#include "shared/NECommonSettings.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEDateTime.hpp"

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/CEVersion.hpp"
#include "areg/src/component/NEService.hpp"

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
    const CEVersion       InterfaceVersion    = "1.0.0";

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
         * \brief   Request ID: MSG_ID_RequestSendMessage
         *          Request to sends message that all clients can see.
         **/
          MSG_ID_RequestSendMessage           = NEService::REQUEST_ID_FIRST     // void RequestSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime );
        /**
         * \brief   Request ID: MSG_ID_RequestKeyTyping
         *          Sent each time when typing a key
         **/
        , MSG_ID_RequestKeyTyping                                               // void RequestKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastSendMessage
         *          The response, sent by connection manager to notify the message typing update
         **/
        , MSG_ID_BroadcastSendMessage         = NEService::RESPONSE_ID_FIRST    // void BroadcastSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastKeyTyping
         *          Sent each time when a client is typing a key
         **/
        , MSG_ID_BroadcastKeyTyping                                             // void BroadcastKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage );
        /**
         * \brief   Broadcast ID: MSG_ID_BroadcastBroadcastMessage
         *          Server broadcasts a message to all clients.
         **/
        , MSG_ID_BroadcastBroadcastMessage                                      // void BroadcastBroadcastMessage( const CEString & serverMessage, const CEDateTime & dateTime );
     
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
    inline const char * GetString( NECentralMessager::eMessageIDs msgId );
     
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
    NECentralMessager::eMessageIDs GetResponseId( NECentralMessager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NECentralMessager::eMessageIDs GetRequestId( NECentralMessager::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_RequestSendMessage
        , MSG_ID_RequestKeyTyping
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

          MSG_ID_BroadcastSendMessage
        , MSG_ID_BroadcastKeyTyping
        , MSG_ID_BroadcastBroadcastMessage
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
        MSG_ID_INVALID  /* There are no attributes. Set Invalid message ID */
    };
    /**
     * \brief   Number of supported Attribute entries
     **/
    const unsigned int NumberofAttributes   = MACRO_ARRAYLEN(AttributeIds);

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
inline const char * NECentralMessager::GetString( NECentralMessager::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NECentralMessager::MSG_ID_RequestSendMessage:
        return "NECentralMessager::MSG_ID_RequestSendMessage";
    case    NECentralMessager::MSG_ID_RequestKeyTyping:
        return "NECentralMessager::MSG_ID_RequestKeyTyping";
    case    NECentralMessager::MSG_ID_BroadcastSendMessage:
        return "NECentralMessager::MSG_ID_BroadcastSendMessage";
    case    NECentralMessager::MSG_ID_BroadcastKeyTyping:
        return "NECentralMessager::MSG_ID_BroadcastKeyTyping";
    case    NECentralMessager::MSG_ID_BroadcastBroadcastMessage:
        return "NECentralMessager::MSG_ID_BroadcastBroadcastMessage";

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
