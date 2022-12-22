//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEHelloService.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEHELLOSERVICE_HPP
#define  GENERATED_SRC_NEHELLOSERVICE_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:24:46 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloService.
 *
 * \file            generated/src/NEHelloService.hpp
 * \ingroup         HelloService Service Interface
 * \brief           This is an automatic generated code of HelloService
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
// NEHelloService namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   HelloService Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          The hello world application
 *
 **/
namespace   NEHelloService
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEHelloService::ServiceName
     **/
    constexpr   char    ServiceName[]   { "HelloService" };
    /**
     * \brief   Implemented Version.
     *          NEHelloService::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

/************************************************************************/
// Service Interface data types
/************************************************************************/


/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEHelloService::eMessageIDs
     *          HelloService Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestHelloService
         *          The request to output greeting.
         **/
          MsgId_requestHelloService   = NEService::REQUEST_ID_FIRST   // void requestHelloService( const String & client );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseHelloService
         *          The response indicating success status to output greeting
         **/
        , MsgId_responseHelloService  = NEService::RESPONSE_ID_FIRST  // void responseHelloService( bool success );
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
     * \brief   Returns string value of NEHelloService::eMessageIDs
     **/
    inline const char * getString( NEHelloService::eMessageIDs msgId );

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
    NEHelloService::eMessageIDs getResponseId( NEHelloService::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEHelloService::eMessageIDs getRequestId( NEHelloService::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEHelloService inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEHelloService methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEHelloService::eMessageIDs );

/**
 * Returns string value of NEHelloService::eMessageIDs
 **/
inline const char * NEHelloService::getString( NEHelloService::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEHelloService::eMessageIDs::MsgId_requestHelloService:
        return "NEHelloService::eMessageIDs::MsgId_requestHelloService";
    case    NEHelloService::eMessageIDs::MsgId_responseHelloService:
        return "NEHelloService::eMessageIDs::MsgId_responseHelloService";

    case NEHelloService::eMessageIDs::MsgId_NotProcessed:
        return "NEHelloService::eMessageIDs::MsgId_NotProcessed";
    case NEHelloService::eMessageIDs::MsgId_Invalid:
        return "NEHelloService::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEHelloService::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEHelloService namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEHELLOSERVICE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEHelloService.hpp file
//////////////////////////////////////////////////////////////////////////
