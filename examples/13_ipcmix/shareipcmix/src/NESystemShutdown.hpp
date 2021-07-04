//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/NESystemShutdown.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHAREIPCMIX_SRC_NESYSTEMSHUTDOWN_HPP
#define  SHAREIPCMIX_SRC_NESYSTEMSHUTDOWN_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 * Generated at     04.07.2021  04:21:26 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/NESystemShutdown.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown Service Interface Namespace declaration.
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
    const char * const    ServiceName         = "SystemShutdown";
    /**
     * \brief   Implemented Version
     *          NESystemShutdown::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   The waiting timeout in milliseconds before shutting down application.
     *          NESystemShutdown::ShutdownWaitTimeout
     **/
    const unsigned int    ShutdownWaitTimeout = 100;

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   Describes the service availability states
     *          NESystemShutdown::eServiceState
     **/
    enum eServiceState
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
    typedef enum E_MessageIDs
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
         * \brief   Broadcast ID: MSG_ID_broadcastServiceUnavailable
         *          Sent to notify the service unavailable state. All clients should be unregistered to start the shutdown procedure.
         **/
          MSG_ID_broadcastServiceUnavailable  = NEService::RESPONSE_ID_FIRST    // void broadcastServiceUnavailable( void );
        /**
         * \brief   Broadcast ID: MSG_ID_broadcastServiceShutdown
         *          Notifies the system is shutting down so that application should disconnect and close.
         **/
        , MSG_ID_broadcastServiceShutdown                                       // void broadcastServiceShutdown( void );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_ServiceState
         *          Describes the current state of service.
         **/
        , MSG_ID_ServiceState                 = NEService::ATTRIBUTE_ID_FIRST   // NESystemShutdown::eServiceState mServiceState;
     
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
    case    NESystemShutdown::ServiceInvalid:
        return "NESystemShutdown::ServiceInvalid";
    case    NESystemShutdown::ServiceReady:
        return "NESystemShutdown::ServiceReady";
    case    NESystemShutdown::ServiceUnavailable:
        return "NESystemShutdown::ServiceUnavailable";
    case    NESystemShutdown::ServiceShutdown:
        return "NESystemShutdown::ServiceShutdown";
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
    case    NESystemShutdown::MSG_ID_broadcastServiceUnavailable:
        return "NESystemShutdown::MSG_ID_broadcastServiceUnavailable";
    case    NESystemShutdown::MSG_ID_broadcastServiceShutdown:
        return "NESystemShutdown::MSG_ID_broadcastServiceShutdown";
    case    NESystemShutdown::MSG_ID_ServiceState:
        return "NESystemShutdown::MSG_ID_ServiceState";

    case NESystemShutdown::MSG_ID_NO_PROCEED:
        return "NESystemShutdown::MSG_ID_NO_PROCEED";
        
    case NESystemShutdown::MSG_ID_INVALID:
        return "NESystemShutdown::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NESystemShutdown::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NESystemShutdown namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHAREIPCMIX_SRC_NESYSTEMSHUTDOWN_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/NESystemShutdown.hpp file
//////////////////////////////////////////////////////////////////////////
