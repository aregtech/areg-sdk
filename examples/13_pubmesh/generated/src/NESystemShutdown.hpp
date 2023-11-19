//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NESystemShutdown.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NESYSTEMSHUTDOWN_HPP
#define  GENERATED_SRC_NESYSTEMSHUTDOWN_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:51:47 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/NESystemShutdown.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
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
// NESystemShutdown namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   SystemShutdown Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          Simple Service Interface to demonstrate working features of AREG SDK.
 *          This service demonstrates the usage of attribute.
 *
 **/
namespace   NESystemShutdown
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NESystemShutdown::ServiceName
     **/
    constexpr   char    ServiceName[]   { "SystemShutdown" };
    /**
     * \brief   Implemented Version.
     *          NESystemShutdown::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   Describes the service availability states
     *          NESystemShutdown::eServiceState
     **/
    enum class eServiceState
    {
        /**
         * \brief   Service shutdown state.
         **/
          ServiceShutdown = 0
        /**
         * \brief   Service is active, ready to communicate
         **/
        , ServiceReady    = 1
    };
    /**
     * \brief   Returns string value of NESystemShutdown::eServiceState
     **/
    inline const char * getString( NESystemShutdown::eServiceState enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NESystemShutdown::eMessageIDs
     *          SystemShutdown Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestSystemShutdown
         *          The request to shutdown the system.
         **/
          MsgId_requestSystemShutdown = NEService::REQUEST_ID_FIRST   // void requestSystemShutdown( void );

    /************************************************************************
     * Response IDs
     ************************************************************************/
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_ServiceState
         *          Describes the system availability state
         **/
        , MsgId_ServiceState          = NEService::ATTRIBUTE_ID_FIRST // NESystemShutdown::eServiceState mServiceState;

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
     * \brief   Returns string value of NESystemShutdown::eMessageIDs
     **/
    inline const char * getString( NESystemShutdown::eMessageIDs msgId );

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
    NESystemShutdown::eMessageIDs getResponseId( NESystemShutdown::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NESystemShutdown::eMessageIDs getRequestId( NESystemShutdown::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NESystemShutdown inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NESystemShutdown methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NESystemShutdown::eMessageIDs );

/************************************************************************
 * NESystemShutdown::eServiceState enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NESystemShutdown::eServiceState )

/**
 * Return string value of NESystemShutdown::eServiceState
 **/
inline const char * NESystemShutdown::getString( NESystemShutdown::eServiceState enumValue )
{
    switch ( enumValue )
    {
    case    NESystemShutdown::eServiceState::ServiceShutdown:
        return "NESystemShutdown::eServiceState::ServiceShutdown";
    case    NESystemShutdown::eServiceState::ServiceReady:
        return "NESystemShutdown::eServiceState::ServiceReady";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NESystemShutdown::eServiceState value!";
    }
}

/**
 * Returns string value of NESystemShutdown::eMessageIDs
 **/
inline const char * NESystemShutdown::getString( NESystemShutdown::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NESystemShutdown::eMessageIDs::MsgId_requestSystemShutdown:
        return "NESystemShutdown::eMessageIDs::MsgId_requestSystemShutdown";
    case    NESystemShutdown::eMessageIDs::MsgId_ServiceState:
        return "NESystemShutdown::eMessageIDs::MsgId_ServiceState";

    case NESystemShutdown::eMessageIDs::MsgId_NotProcessed:
        return "NESystemShutdown::eMessageIDs::MsgId_NotProcessed";
    case NESystemShutdown::eMessageIDs::MsgId_Invalid:
        return "NESystemShutdown::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NESystemShutdown::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NESystemShutdown namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NESYSTEMSHUTDOWN_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NESystemShutdown.hpp file
//////////////////////////////////////////////////////////////////////////
