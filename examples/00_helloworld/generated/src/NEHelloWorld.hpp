//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
#pragma once

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     20.10.2021  15:43:41 GMT+02:00 
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
 *          The hello world application
 *
 **/
namespace   NEHelloWorld
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NEHelloWorld::ServiceName
     **/
    constexpr char        ServiceName[]       { "HelloWorld" };
    /**
     * \brief   Implemented Version
     *          NEHelloWorld::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

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
         *          The request to output greeting.
         **/
          MsgId_requestHelloWorld     = NEService::REQUEST_ID_FIRST     // void requestHelloWorld( const String & client );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseHelloWorld
         *          The response indicating success status to output greeting
         **/
        , MsgId_responseHelloWorld    = NEService::RESPONSE_ID_FIRST    // void responseHelloWorld( bool success );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed          = NEService::INVALID_MESSAGE_ID  
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid               = NEService::RESPONSE_ID_NONE    
     
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
    case    NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        return "NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld";

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

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEHelloWorld.hpp file
//////////////////////////////////////////////////////////////////////////
