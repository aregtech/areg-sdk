//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEHelloUnblock.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEHELLOUNBLOCK_HPP
#define  GENERATED_SRC_NEHELLOUNBLOCK_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:56 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnblock.
 *
 * \file            generated/src/NEHelloUnblock.hpp
 * \ingroup         HelloUnblock Service Interface
 * \brief           This is an automatic generated code of HelloUnblock
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////

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
// NEHelloUnblock namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   HelloUnblock Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *          In this example the client sends a request each 200 ms and the service replies each 500,
 *          so that when the client sends next request, there is always a pending request on the service.
 *          To process the request and make correct reply, the service manually unblocks the request
 *          and when timeout is expired, sends the respons.
 *          The service is stopped and the application is shutdown when there is no more pending request.
 *
 **/
namespace   NEHelloUnblock
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEHelloUnblock::ServiceName
     **/
    constexpr   char    ServiceName[]   { "HelloUnblock" };
    /**
     * \brief   Implemented Version.
     *          NEHelloUnblock::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The maximum number of messages to output.
     *          NEHelloUnblock::MaxMessages
     **/
    constexpr short           MaxMessages         = 100;

    /**
     * \brief   The timeout in milliseconds where the client send a request
     *          NEHelloUnblock::ClientTimeot
     **/
    constexpr unsigned int    ClientTimeot        = 200;

    /**
     * \brief   Timeout where the service replies with the response
     *          NEHelloUnblock::ServiceTimeout
     **/
    constexpr unsigned int    ServiceTimeout      = 500;

    /**
     * \brief   The invalid ID
     *          NEHelloUnblock::InvalidId
     **/
    constexpr unsigned int    InvalidId           = 0;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The state of the service
     *          NEHelloUnblock::eServiceState
     **/
    enum class eServiceState
    {
        /**
         * \brief   Invalid undefined state
         **/
          ServiceUndefined
        /**
         * \brief   Service is up and running, ready to process.
         **/
        , ServiceActive
        /**
         * \brief   Service completed job and will not process anymore. 
         **/
        , ServiceShutdown
    };
    /**
     * \brief   Returns string value of NEHelloUnblock::eServiceState
     **/
    inline const char * getString( NEHelloUnblock::eServiceState enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEHelloUnblock::eMessageIDs
     *          HelloUnblock Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestIdentifier
         *          Request to assign an ID to the client used to to call unblock request.
         **/
          MsgId_requestIdentifier     = NEService::REQUEST_ID_FIRST   // void requestIdentifier( void );
        /**
         * \brief   Request ID: MsgId_requestHelloUblock
         *          Request to print hello world
         **/
        , MsgId_requestHelloUblock                                      // void requestHelloUblock( unsigned int clientId, unsigned int seqNr );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseIdentifier
         *          Sent to set ID for client.
         **/
        , MsgId_responseIdentifier    = NEService::RESPONSE_ID_FIRST  // void responseIdentifier( unsigned int clientId );
        /**
         * \brief   Response ID: MsgId_responseHelloUnblock
         *          The response to hello world request.
         **/
        , MsgId_responseHelloUnblock                                    // void responseHelloUnblock( unsigned int clientId, unsigned int seqNr );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_HelloServiceState
         *          The state of the service.
         **/
        , MsgId_HelloServiceState     = NEService::ATTRIBUTE_ID_FIRST // NEHelloUnblock::eServiceState mHelloServiceState;

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
     * \brief   Returns string value of NEHelloUnblock::eMessageIDs
     **/
    inline const char * getString( NEHelloUnblock::eMessageIDs msgId );

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
    NEHelloUnblock::eMessageIDs getResponseId( NEHelloUnblock::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEHelloUnblock::eMessageIDs getRequestId( NEHelloUnblock::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEHelloUnblock inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEHelloUnblock methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEHelloUnblock::eMessageIDs );

/************************************************************************
 * NEHelloUnblock::eServiceState enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEHelloUnblock::eServiceState )

/**
 * Return string value of NEHelloUnblock::eServiceState
 **/
inline const char * NEHelloUnblock::getString( NEHelloUnblock::eServiceState enumValue )
{
    switch ( enumValue )
    {
    case    NEHelloUnblock::eServiceState::ServiceUndefined:
        return "NEHelloUnblock::eServiceState::ServiceUndefined";
    case    NEHelloUnblock::eServiceState::ServiceActive:
        return "NEHelloUnblock::eServiceState::ServiceActive";
    case    NEHelloUnblock::eServiceState::ServiceShutdown:
        return "NEHelloUnblock::eServiceState::ServiceShutdown";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEHelloUnblock::eServiceState value!";
    }
}

/**
 * Returns string value of NEHelloUnblock::eMessageIDs
 **/
inline const char * NEHelloUnblock::getString( NEHelloUnblock::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEHelloUnblock::eMessageIDs::MsgId_requestIdentifier:
        return "NEHelloUnblock::eMessageIDs::MsgId_requestIdentifier";
    case    NEHelloUnblock::eMessageIDs::MsgId_requestHelloUblock:
        return "NEHelloUnblock::eMessageIDs::MsgId_requestHelloUblock";
    case    NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier:
        return "NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier";
    case    NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock:
        return "NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock";
    case    NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState:
        return "NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState";

    case NEHelloUnblock::eMessageIDs::MsgId_NotProcessed:
        return "NEHelloUnblock::eMessageIDs::MsgId_NotProcessed";
    case NEHelloUnblock::eMessageIDs::MsgId_Invalid:
        return "NEHelloUnblock::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEHelloUnblock::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEHelloUnblock namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEHELLOUNBLOCK_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEHelloUnblock.hpp file
//////////////////////////////////////////////////////////////////////////
