//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NESystemShutdown.hpp file
//////////////////////////////////////////////////////////////////////////
#pragma once

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:15 GMT+02:00 
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
 *          This service demonstrates the usage in combination with other services.
 *          It also can be used by any other object that has interest in system shutdown state.
 *          System run and shutdown service.
 *
 **/
namespace   NESystemShutdown
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NESystemShutdown::ServiceName
     **/
    constexpr char        ServiceName[]       { "SystemShutdown" };
    /**
     * \brief   Implemented Version
     *          NESystemShutdown::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   The waiting timeout in milliseconds before shutting down application.
     *          NESystemShutdown::ShutdownWaitTimeout
     **/
    constexpr unsigned int    ShutdownWaitTimeout = 100;

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
         * \brief   The service is invalid, not ready to receive messages.
         **/
          ServiceInvalid      
        /**
         * \brief   Service is active, ready to communicate
         **/
        , ServiceReady        
        /**
         * \brief   Service is unavailable
         **/
        , ServiceUnavailable  
        /**
         * \brief   Service shutting down.
         **/
        , ServiceShutdown     
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
     
    /************************************************************************
     * Response IDs
     ************************************************************************/     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
        /**
         * \brief   Broadcast ID: MsgId_broadcastServiceUnavailable
         *          Sent to notify the service unavailable state. All clients should be unregistered to start the shutdown procedure.
         **/
          MsgId_broadcastServiceUnavailable   = NEService::RESPONSE_ID_FIRST    // void broadcastServiceUnavailable( void );
        /**
         * \brief   Broadcast ID: MsgId_broadcastServiceShutdown
         *          Notifies the system is shutting down so that application should disconnect and close.
         **/
        , MsgId_broadcastServiceShutdown                                        // void broadcastServiceShutdown( void );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_ServiceState
         *          Describes the current state of service.
         **/
        , MsgId_ServiceState                  = NEService::ATTRIBUTE_ID_FIRST   // NESystemShutdown::eServiceState mServiceState;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed                  = NEService::INVALID_MESSAGE_ID  
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                       = NEService::RESPONSE_ID_NONE    
     
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
    case    NESystemShutdown::eServiceState::ServiceInvalid:
        return "NESystemShutdown::eServiceState::ServiceInvalid";
    case    NESystemShutdown::eServiceState::ServiceReady:
        return "NESystemShutdown::eServiceState::ServiceReady";
    case    NESystemShutdown::eServiceState::ServiceUnavailable:
        return "NESystemShutdown::eServiceState::ServiceUnavailable";
    case    NESystemShutdown::eServiceState::ServiceShutdown:
        return "NESystemShutdown::eServiceState::ServiceShutdown";
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
    case    NESystemShutdown::eMessageIDs::MsgId_broadcastServiceUnavailable:
        return "NESystemShutdown::eMessageIDs::MsgId_broadcastServiceUnavailable";
    case    NESystemShutdown::eMessageIDs::MsgId_broadcastServiceShutdown:
        return "NESystemShutdown::eMessageIDs::MsgId_broadcastServiceShutdown";
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

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NESystemShutdown.hpp file
//////////////////////////////////////////////////////////////////////////
