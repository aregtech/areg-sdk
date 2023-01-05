//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEHELLOWORLD_HPP
#define  GENERATED_SRC_NEHELLOWORLD_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:07:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/NEHelloWorld.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////
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
// NEHelloWorld namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   HelloWorld Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *
 **/
namespace   NEHelloWorld
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEHelloWorld::ServiceName
     **/
    constexpr   char    ServiceName[]   { "HelloWorld" };
    /**
     * \brief   Implemented Version.
     *          NEHelloWorld::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The maximum number of messages to output.
     *          NEHelloWorld::MaxMessages
     **/
    constexpr short           MaxMessages         = 37;

/************************************************************************/
// Service Interface data types
/************************************************************************/


/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEHelloWorld::eMessageIDs
     *          HelloWorld Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestHelloWorld
         *          Request to print hello world
         **/
          MsgId_requestHelloWorld         = NEService::REQUEST_ID_FIRST   // void requestHelloWorld( const String & roleName );
        /**
         * \brief   Request ID: MsgId_requestShutdownService
         *          Sent by client to request to shutdown service and the application. This request has no response.
         **/
        , MsgId_requestShutdownService                                      // void requestShutdownService( void );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseHelloWorld
         *          The response to hello world request.
         **/
        , MsgId_responseHelloWorld        = NEService::RESPONSE_ID_FIRST  // void responseHelloWorld( void );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MsgId_broadcastReachedMaximum
         *          Triggered to notify that  reached the maximum number of requests.
         **/
        , MsgId_broadcastReachedMaximum                                     // void broadcastReachedMaximum( int maxNumber );

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
     * \brief   Returns string value of NEHelloWorld::eMessageIDs
     **/
    inline const char * getString( NEHelloWorld::eMessageIDs msgId );

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
    NEHelloWorld::eMessageIDs getResponseId( NEHelloWorld::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEHelloWorld::eMessageIDs getRequestId( NEHelloWorld::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEHelloWorld inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEHelloWorld methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEHelloWorld::eMessageIDs );

/**
 * Returns string value of NEHelloWorld::eMessageIDs
 **/
inline const char * NEHelloWorld::getString( NEHelloWorld::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
        return "NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld";
    case    NEHelloWorld::eMessageIDs::MsgId_requestShutdownService:
        return "NEHelloWorld::eMessageIDs::MsgId_requestShutdownService";
    case    NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        return "NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld";
    case    NEHelloWorld::eMessageIDs::MsgId_broadcastReachedMaximum:
        return "NEHelloWorld::eMessageIDs::MsgId_broadcastReachedMaximum";

    case NEHelloWorld::eMessageIDs::MsgId_NotProcessed:
        return "NEHelloWorld::eMessageIDs::MsgId_NotProcessed";
    case NEHelloWorld::eMessageIDs::MsgId_Invalid:
        return "NEHelloWorld::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEHelloWorld::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEHelloWorld namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEHELLOWORLD_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
