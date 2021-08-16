//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEPowerManager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEPOWERMANAGER_HPP
#define  GENERATED_SRC_NEPOWERMANAGER_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PowerManager.
 * Generated at     15.08.2021  00:03:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/NEPowerManager.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager Service Interface Namespace declaration.
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
// NEPowerManager namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   PowerManager Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          This is a power manager of Traffic Light Controller. It powers the lights ON and OFF.
 *
 **/
namespace   NEPowerManager
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NEPowerManager::ServiceName
     **/
    const char * const    ServiceName         = "PowerManager";
    /**
     * \brief   Implemented Version
     *          NEPowerManager::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   The power state of the traffic lights.
     *          NEPowerManager::ePoweredState
     **/
    enum ePoweredState
    {
        /**
         * \brief   The lights are OFF
         **/
          LightsOFF   
        /**
         * \brief   The lights are ON.
         **/
        , LightsON    
    };
    /**
     * \brief   Returns string value of NEPowerManager::ePoweredState
     **/
    inline const char * getString( NEPowerManager::ePoweredState enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEPowerManager::eMessageIDs
     *          PowerManager Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestPowerOn
         *          Called to power ON the traffic lights.
         **/
          MSG_ID_requestPowerOn               = NEService::REQUEST_ID_FIRST     // void requestPowerOn( void );
        /**
         * \brief   Request ID: MSG_ID_requestPowerOff
         *          Called to power OFF the traffic lights.
         **/
        , MSG_ID_requestPowerOff                                                // void requestPowerOff( void );
        /**
         * \brief   Request ID: MSG_ID_requestStartTrafficLight
         *          Triggered to start the traffic light
         **/
        , MSG_ID_requestStartTrafficLight                                       // void requestStartTrafficLight( void );
        /**
         * \brief   Request ID: MSG_ID_requestStopTrafficLight
         *          Call to stop the traffic lights and put them in initialization state.
         **/
        , MSG_ID_requestStopTrafficLight                                         // void requestStopTrafficLight( void );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_responseStartTrafficLight
         *          Sent as a response to start the traffic light. The traffic light can be started when it is powered ON and in initialization state.
         **/
        , MSG_ID_responseStartTrafficLight    = NEService::RESPONSE_ID_FIRST    // void responseStartTrafficLight( bool Success );
        /**
         * \brief   Response ID: MSG_ID_responseStopTrafficLight
         *          Response to stop the traffic lights.
         **/
        , MSG_ID_responseStopTrafficLight                                        // void responseStopTrafficLight( bool Success );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_LightsPowerState
         *          The power state of the traffic lights.
         **/
        , MSG_ID_LightsPowerState             = NEService::ATTRIBUTE_ID_FIRST   // NEPowerManager::ePoweredState mLightsPowerState;
     
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
     * \brief   Returns string value of NEPowerManager::eMessageIDs
     **/
    inline const char * getString( NEPowerManager::eMessageIDs msgId );
     
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
    NEPowerManager::eMessageIDs getResponseId( NEPowerManager::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEPowerManager::eMessageIDs getRequestId( NEPowerManager::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEPowerManager inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEPowerManager methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEPowerManager::eMessageIDs );

/************************************************************************
 * NEPowerManager::ePoweredState enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEPowerManager::ePoweredState )

/**
 * Return string value of NEPowerManager::ePoweredState
 **/
inline const char * NEPowerManager::getString( NEPowerManager::ePoweredState enumValue )
{
    switch ( enumValue )
    {
    case    NEPowerManager::LightsOFF:
        return "NEPowerManager::LightsOFF";
    case    NEPowerManager::LightsON:
        return "NEPowerManager::LightsON";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEPowerManager::ePoweredState value!";
    }
}

/**
 * Returns string value of NEPowerManager::eMessageIDs
 **/
inline const char * NEPowerManager::getString( NEPowerManager::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEPowerManager::MSG_ID_requestPowerOn:
        return "NEPowerManager::MSG_ID_requestPowerOn";
    case    NEPowerManager::MSG_ID_requestPowerOff:
        return "NEPowerManager::MSG_ID_requestPowerOff";
    case    NEPowerManager::MSG_ID_requestStartTrafficLight:
        return "NEPowerManager::MSG_ID_requestStartTrafficLight";
    case    NEPowerManager::MSG_ID_requestStopTrafficLight:
        return "NEPowerManager::MSG_ID_requestStopTrafficLight";
    case    NEPowerManager::MSG_ID_responseStartTrafficLight:
        return "NEPowerManager::MSG_ID_responseStartTrafficLight";
    case    NEPowerManager::MSG_ID_responseStopTrafficLight:
        return "NEPowerManager::MSG_ID_responseStopTrafficLight";
    case    NEPowerManager::MSG_ID_LightsPowerState:
        return "NEPowerManager::MSG_ID_LightsPowerState";

    case NEPowerManager::MSG_ID_NO_PROCEED:
        return "NEPowerManager::MSG_ID_NO_PROCEED";
        
    case NEPowerManager::MSG_ID_INVALID:
        return "NEPowerManager::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEPowerManager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEPowerManager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_NEPOWERMANAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEPowerManager.hpp file
//////////////////////////////////////////////////////////////////////////
