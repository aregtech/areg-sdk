//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEPowerManager.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEPOWERMANAGER_HPP
#define  GENERATED_SRC_NEPOWERMANAGER_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PowerManager.
 *
 * \file            generated/src/NEPowerManager.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager
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
     * \brief   Service Interface Name.
     *          NEPowerManager::ServiceName
     **/
    constexpr   char    ServiceName[]   { "PowerManager" };
    /**
     * \brief   Implemented Version.
     *          NEPowerManager::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The power state of the traffic lights.
     *          NEPowerManager::ePoweredState
     **/
    enum class ePoweredState
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
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestPowerOn
         *          Called to power ON the traffic lights.
         **/
          MsgId_requestPowerOn            = NEService::REQUEST_ID_FIRST   // void requestPowerOn( void );
        /**
         * \brief   Request ID: MsgId_requestPowerOff
         *          Called to power OFF the traffic lights.
         **/
        , MsgId_requestPowerOff                                             // void requestPowerOff( void );
        /**
         * \brief   Request ID: MsgId_requestStartTrafficLight
         *          Triggered to start the traffic light
         **/
        , MsgId_requestStartTrafficLight                                    // void requestStartTrafficLight( void );
        /**
         * \brief   Request ID: MsgId_requestStopTrafficLight
         *          Call to stop the traffic lights and put them in initialization state.
         **/
        , MsgId_requestStopTrafficLight                                     // void requestStopTrafficLight( void );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseStartTrafficLight
         *          Sent as a response to start the traffic light. The traffic light can be started when it is powered ON and in initialization state.
         **/
        , MsgId_responseStartTrafficLight = NEService::RESPONSE_ID_FIRST  // void responseStartTrafficLight( bool Success );
        /**
         * \brief   Response ID: MsgId_responseStopTrafficLight
         *          Response to stop the traffic lights.
         **/
        , MsgId_responseStopTrafficLight                                    // void responseStopTrafficLight( bool Success );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_LightsPowerState
         *          The power state of the traffic lights.
         **/
        , MsgId_LightsPowerState          = NEService::ATTRIBUTE_ID_FIRST // NEPowerManager::ePoweredState mLightsPowerState;

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
    case    NEPowerManager::ePoweredState::LightsOFF:
        return "NEPowerManager::ePoweredState::LightsOFF";
    case    NEPowerManager::ePoweredState::LightsON:
        return "NEPowerManager::ePoweredState::LightsON";
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
    case    NEPowerManager::eMessageIDs::MsgId_requestPowerOn:
        return "NEPowerManager::eMessageIDs::MsgId_requestPowerOn";
    case    NEPowerManager::eMessageIDs::MsgId_requestPowerOff:
        return "NEPowerManager::eMessageIDs::MsgId_requestPowerOff";
    case    NEPowerManager::eMessageIDs::MsgId_requestStartTrafficLight:
        return "NEPowerManager::eMessageIDs::MsgId_requestStartTrafficLight";
    case    NEPowerManager::eMessageIDs::MsgId_requestStopTrafficLight:
        return "NEPowerManager::eMessageIDs::MsgId_requestStopTrafficLight";
    case    NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight:
        return "NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight";
    case    NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight:
        return "NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight";
    case    NEPowerManager::eMessageIDs::MsgId_LightsPowerState:
        return "NEPowerManager::eMessageIDs::MsgId_LightsPowerState";

    case NEPowerManager::eMessageIDs::MsgId_NotProcessed:
        return "NEPowerManager::eMessageIDs::MsgId_NotProcessed";
    case NEPowerManager::eMessageIDs::MsgId_Invalid:
        return "NEPowerManager::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEPowerManager::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEPowerManager namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEPOWERMANAGER_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEPowerManager.hpp file
//////////////////////////////////////////////////////////////////////////
