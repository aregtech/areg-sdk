//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NESimpleTrafficLight.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NESIMPLETRAFFICLIGHT_HPP
#define  GENERATED_SRC_NESIMPLETRAFFICLIGHT_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     23.12.2022  00:41:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 *
 * \file            generated/src/NESimpleTrafficLight.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight
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
// NESimpleTrafficLight namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   SimpleTrafficLight Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          The simple traffic light that triggers timeout and changes traffic light state.
 *
 **/
namespace   NESimpleTrafficLight
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NESimpleTrafficLight::ServiceName
     **/
    constexpr   char    ServiceName[]   { "SimpleTrafficLight" };
    /**
     * \brief   Implemented Version.
     *          NESimpleTrafficLight::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

    /**
     * \brief   The red light timeout in milliseconds.
     *          NESimpleTrafficLight::TimeoutRed
     **/
    constexpr unsigned int    TimeoutRed          = 3000;

    /**
     * \brief   The yellow light timeout in milliseconds.
     *          NESimpleTrafficLight::TimeoutYellow
     **/
    constexpr unsigned int    TimeoutYellow       = 1000;

    /**
     * \brief   The green light timeout in milliseconds.
     *          NESimpleTrafficLight::TimeoutGreen
     **/
    constexpr unsigned int    TimeoutGreen        = 3000;

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   The simple traffic light states
     *          NESimpleTrafficLight::eTrafficLight
     **/
    enum class eTrafficLight
    {
        /**
         * \brief   The light is off.
         **/
          LightOff
        /**
         * \brief   The light is red.
         **/
        , LightRed
        /**
         * \brief   The light is yellow.
         **/
        , LightYellow
        /**
         * \brief   The light is green.
         **/
        , LightGreen
    };
    /**
     * \brief   Returns string value of NESimpleTrafficLight::eTrafficLight
     **/
    inline const char * getString( NESimpleTrafficLight::eTrafficLight enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NESimpleTrafficLight::eMessageIDs
     *          SimpleTrafficLight Service interface defined message IDs
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

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_SouthNorth
         *          The light state of south-north traffic.
         **/
          MsgId_SouthNorth    = NEService::ATTRIBUTE_ID_FIRST // NESimpleTrafficLight::eTrafficLight mSouthNorth;
        /**
         * \brief   Attribute ID: MsgId_EastWest
         *          The light state of east-west traffic.
         **/
        , MsgId_EastWest                                        // NESimpleTrafficLight::eTrafficLight mEastWest;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed  = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid       = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NESimpleTrafficLight::eMessageIDs
     **/
    inline const char * getString( NESimpleTrafficLight::eMessageIDs msgId );

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
    NESimpleTrafficLight::eMessageIDs getResponseId( NESimpleTrafficLight::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NESimpleTrafficLight::eMessageIDs getRequestId( NESimpleTrafficLight::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficLight inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NESimpleTrafficLight methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NESimpleTrafficLight::eMessageIDs );

/************************************************************************
 * NESimpleTrafficLight::eTrafficLight enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NESimpleTrafficLight::eTrafficLight )

/**
 * Return string value of NESimpleTrafficLight::eTrafficLight
 **/
inline const char * NESimpleTrafficLight::getString( NESimpleTrafficLight::eTrafficLight enumValue )
{
    switch ( enumValue )
    {
    case    NESimpleTrafficLight::eTrafficLight::LightOff:
        return "NESimpleTrafficLight::eTrafficLight::LightOff";
    case    NESimpleTrafficLight::eTrafficLight::LightRed:
        return "NESimpleTrafficLight::eTrafficLight::LightRed";
    case    NESimpleTrafficLight::eTrafficLight::LightYellow:
        return "NESimpleTrafficLight::eTrafficLight::LightYellow";
    case    NESimpleTrafficLight::eTrafficLight::LightGreen:
        return "NESimpleTrafficLight::eTrafficLight::LightGreen";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NESimpleTrafficLight::eTrafficLight value!";
    }
}

/**
 * Returns string value of NESimpleTrafficLight::eMessageIDs
 **/
inline const char * NESimpleTrafficLight::getString( NESimpleTrafficLight::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth:
        return "NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth";
    case    NESimpleTrafficLight::eMessageIDs::MsgId_EastWest:
        return "NESimpleTrafficLight::eMessageIDs::MsgId_EastWest";

    case NESimpleTrafficLight::eMessageIDs::MsgId_NotProcessed:
        return "NESimpleTrafficLight::eMessageIDs::MsgId_NotProcessed";
    case NESimpleTrafficLight::eMessageIDs::MsgId_Invalid:
        return "NESimpleTrafficLight::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NESimpleTrafficLight::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficLight namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NESIMPLETRAFFICLIGHT_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NESimpleTrafficLight.hpp file
//////////////////////////////////////////////////////////////////////////
