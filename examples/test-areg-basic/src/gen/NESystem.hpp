//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/NESystem.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_NESYSTEM_HPP
#define  SRC_GEN_NESYSTEM_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     12.05.2021  16:41:13 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/NESystem.hpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Namespace declaration.
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
// NESystem namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   System Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          The system state.
 *
 **/
namespace   NESystem
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NESystem::ServiceName
     **/
    const char * const    ServiceName         = "System";
    /**
     * \brief   Implemented Version
     *          NESystem::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "0.1.0";

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   The description of system states.
     *          NESystem::eSystemState
     **/
    enum eSystemState
    {
        /**
         * \brief   Undefined state
         **/
          SystemUndefined = 0
        /**
         * \brief   System power is off. Normally, when user requests to quit to shuts down
         **/
        , SystemPowerOff  = 1
        /**
         * \brief   System power is on. When starts and during the entire time of system run.
         **/
        , SystemPowerOn   = 2
        /**
         * \brief   System start, all service run, power on bit-field is set.
         **/
        , SystemStart     = 6
        /**
         * \brief   User requested to shutdown. Power on to power off bit-fields are set.
         **/
        , SystemShutdown  = 11
        /**
         * \brief   System started and running. Power on and system start bit-fields are set.
         **/
        , SystemRunning   = 22
    };
    /**
     * \brief   Returns string value of NESystem::eSystemState
     **/
    inline const char * getString( NESystem::eSystemState enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NESystem::eMessageIDs
     *          System Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestPowerOff
         *          Requested to power off.
         **/
          MSG_ID_requestPowerOff  = NEService::REQUEST_ID_FIRST     // void requestPowerOff( void );
        /**
         * \brief   Request ID: MSG_ID_requestPowerOn
         *          requested to power on
         **/
        , MSG_ID_requestPowerOn                                     // void requestPowerOn( void );
        /**
         * \brief   Request ID: MSG_ID_requestShutdown
         *          requested to shutdown
         **/
        , MSG_ID_requestShutdown                                    // void requestShutdown( void );
        /**
         * \brief   Request ID: MSG_ID_requestStart
         *          requested to start.
         **/
        , MSG_ID_requestStart                                       // void requestStart( void );
     
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
         * \brief   Attribute ID: MSG_ID_SystemState
         *          The system state.
         **/
        , MSG_ID_SystemState      = NEService::ATTRIBUTE_ID_FIRST   // NESystem::eSystemState mSystemState;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED       = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID          = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
    /**
     * \brief   Returns string value of NESystem::eMessageIDs
     **/
    inline const char * getString( NESystem::eMessageIDs msgId );
     
/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/
 
    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & createInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NESystem::eMessageIDs getResponseId( NESystem::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NESystem::eMessageIDs getRequestId( NESystem::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_requestPowerOff
        , MSG_ID_requestPowerOn
        , MSG_ID_requestShutdown
        , MSG_ID_requestStart
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
        MSG_ID_INVALID  /* There are no responses. Set Invalid message ID */
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
          MSG_ID_SystemState
    };
    /**
     * \brief   Number of supported Attribute entries
     **/
    const unsigned int NumberofAttributes   = MACRO_ARRAYLEN(AttributeIds);

}
//////////////////////////////////////////////////////////////////////////
// NESystem inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NESystem methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NESystem::eMessageIDs );

/************************************************************************
 * NESystem::eSystemState enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NESystem::eSystemState )

/**
 * Return string value of NESystem::eSystemState
 **/
inline const char * NESystem::getString( NESystem::eSystemState enumValue )
{
    switch ( enumValue )
    {
    case    NESystem::SystemUndefined:
        return "NESystem::SystemUndefined";
    case    NESystem::SystemPowerOff:
        return "NESystem::SystemPowerOff";
    case    NESystem::SystemPowerOn:
        return "NESystem::SystemPowerOn";
    case    NESystem::SystemStart:
        return "NESystem::SystemStart";
    case    NESystem::SystemShutdown:
        return "NESystem::SystemShutdown";
    case    NESystem::SystemRunning:
        return "NESystem::SystemRunning";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NESystem::eSystemState value!";
    }
}

/**
 * Returns string value of NESystem::eMessageIDs
 **/
inline const char * NESystem::getString( NESystem::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NESystem::MSG_ID_requestPowerOff:
        return "NESystem::MSG_ID_requestPowerOff";
    case    NESystem::MSG_ID_requestPowerOn:
        return "NESystem::MSG_ID_requestPowerOn";
    case    NESystem::MSG_ID_requestShutdown:
        return "NESystem::MSG_ID_requestShutdown";
    case    NESystem::MSG_ID_requestStart:
        return "NESystem::MSG_ID_requestStart";
    case    NESystem::MSG_ID_SystemState:
        return "NESystem::MSG_ID_SystemState";

    case NESystem::MSG_ID_NO_PROCEED:
        return "NESystem::MSG_ID_NO_PROCEED";
        
    case NESystem::MSG_ID_INVALID:
        return "NESystem::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NESystem::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NESystem namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SRC_GEN_NESYSTEM_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/NESystem.hpp file
//////////////////////////////////////////////////////////////////////////
