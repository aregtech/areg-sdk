//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NESimpleTrafficLight.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NESIMPLETRAFFICLIGHT_HPP
#define  GENERATED_SRC_NESIMPLETRAFFICLIGHT_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 * Generated at     11.08.2021  13:18:20 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/NESimpleTrafficLight.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight Service Interface Namespace declaration.
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
     * \brief   Service Interface Name
     *          NESimpleTrafficLight::ServiceName
     **/
    const char * const    ServiceName         = "SimpleTrafficLight";
    /**
     * \brief   Implemented Version
     *          NESimpleTrafficLight::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

    /**
     * \brief   The red light timeout in milliseconds.
     *          NESimpleTrafficLight::TimeoutRed
     **/
    const unsigned int    TimeoutRed          = 10000;

    /**
     * \brief   The yellow light timeout in milliseconds.
     *          NESimpleTrafficLight::TimeoutYellow
     **/
    const unsigned int    TimeoutYellow       = 4000;

    /**
     * \brief   The green light timeout in milliseconds.
     *          NESimpleTrafficLight::TimeoutGreen
     **/
    const unsigned int    TimeoutGreen        = 10000;

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   The simple traffic light states
     *          NESimpleTrafficLight::eTrafficLight
     **/
    enum eTrafficLight
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
         * \brief   Broadcast ID: MSG_ID_broadcastLightChanged
         *          Triggered when the light state of traffic is changed.
         **/
          MSG_ID_broadcastLightChanged    = NEService::RESPONSE_ID_FIRST    // void broadcastLightChanged( NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest );
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_SouthNorth
         *          The light state of south-north traffic.
         **/
        , MSG_ID_SouthNorth               = NEService::ATTRIBUTE_ID_FIRST   // NESimpleTrafficLight::eTrafficLight mSouthNorth;
        /**
         * \brief   Attribute ID: MSG_ID_EastWest
         *          The light state of east-west traffic.
         **/
        , MSG_ID_EastWest                                                   // NESimpleTrafficLight::eTrafficLight mEastWest;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED               = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID                  = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
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
    case    NESimpleTrafficLight::LightOff:
        return "NESimpleTrafficLight::LightOff";
    case    NESimpleTrafficLight::LightRed:
        return "NESimpleTrafficLight::LightRed";
    case    NESimpleTrafficLight::LightYellow:
        return "NESimpleTrafficLight::LightYellow";
    case    NESimpleTrafficLight::LightGreen:
        return "NESimpleTrafficLight::LightGreen";
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
    case    NESimpleTrafficLight::MSG_ID_broadcastLightChanged:
        return "NESimpleTrafficLight::MSG_ID_broadcastLightChanged";
    case    NESimpleTrafficLight::MSG_ID_SouthNorth:
        return "NESimpleTrafficLight::MSG_ID_SouthNorth";
    case    NESimpleTrafficLight::MSG_ID_EastWest:
        return "NESimpleTrafficLight::MSG_ID_EastWest";

    case NESimpleTrafficLight::MSG_ID_NO_PROCEED:
        return "NESimpleTrafficLight::MSG_ID_NO_PROCEED";
        
    case NESimpleTrafficLight::MSG_ID_INVALID:
        return "NESimpleTrafficLight::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NESimpleTrafficLight::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficLight namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_NESIMPLETRAFFICLIGHT_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NESimpleTrafficLight.hpp file
//////////////////////////////////////////////////////////////////////////
