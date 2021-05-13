//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/NETrafficLight.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_NETRAFFICLIGHT_HPP
#define  SRC_GEN_NETRAFFICLIGHT_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/NETrafficLight.hpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Namespace declaration.
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
// NETrafficLight namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   TrafficLight Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          The traffic light to change lights.
 *
 **/
namespace   NETrafficLight
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NETrafficLight::ServiceName
     **/
    const char * const    ServiceName         = "TrafficLight";
    /**
     * \brief   Implemented Version
     *          NETrafficLight::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "0.1.0";

    /**
     * \brief   Timeout to remain in red -- 5000 ms (5 secs)
     *          NETrafficLight::TIMEOUT_RED
     **/
    const unsigned int    TIMEOUT_RED         = 5000;

    /**
     * \brief   Timeout to remain in yellow, 3000 milliseconds (3 secs)
     *          NETrafficLight::TIMEOUT_YELLOW
     **/
    const unsigned int    TIMEOUT_YELLOW      = 3000;

    /**
     * \brief   The timeout in milliseconds to wait in Green and Red (6500 ms = 6.5 secs), 
     *          i.e. in Red-Yellow it will remain 1500 ms (1.5 secs).
     *          NETrafficLight::TIMEOUT_CHANGE
     **/
    const unsigned int    TIMEOUT_CHANGE      = 6500;

/************************************************************************/
// Service Interface data types
/************************************************************************/

    
    /**
     * \brief   The colors of traffic light.
     *          NETrafficLight::eLight
     **/
    enum eLight
    {
        /**
         * \brief   No color, the light is off
         **/
          LightOff    
        /**
         * \brief   Red color of light
         **/
        , Red         
        /**
         * \brief   Red and Yellow color of light.
         **/
        , RedYellow   
        /**
         * \brief   The yellow color of the light.
         **/
        , Yellow      
        /**
         * \brief   Green color of the light.
         **/
        , Green       
    };
    /**
     * \brief   Returns string value of NETrafficLight::eLight
     **/
    inline const char * getString( NETrafficLight::eLight enumValue );

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NETrafficLight::eMessageIDs
     *          TrafficLight Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestChangeLight
         *          Requested rapidly to change the light.
         **/
          MSG_ID_requestChangeLight   = NEService::REQUEST_ID_FIRST     // void requestChangeLight( NETrafficLight::eLight lightColor, bool holdon );
     
    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MSG_ID_responseChangeLight
         *          Resonse to change the light color.
         **/
        , MSG_ID_responseChangeLight  = NEService::RESPONSE_ID_FIRST    // void responseChangeLight( NETrafficLight::eLight lightColor );     
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/
     
    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MSG_ID_TrafficLight
         *          The current color of traffic light
         **/
        , MSG_ID_TrafficLight         = NEService::ATTRIBUTE_ID_FIRST   // NETrafficLight::eLight mTrafficLight;
     
    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MSG_ID_NO_PROCEED           = NEService::RESPONSE_ID_NONE    
        /**
         * \brief   ID of invalid call
         **/
        , MSG_ID_INVALID              = NEService::INVALID_MESSAGE_ID  
     
    } eMessageIDs;
    /**
     * \brief   Returns string value of NETrafficLight::eMessageIDs
     **/
    inline const char * getString( NETrafficLight::eMessageIDs msgId );
     
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
    NETrafficLight::eMessageIDs getResponseId( NETrafficLight::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NETrafficLight::eMessageIDs getRequestId( NETrafficLight::eMessageIDs respId );

    /**
     * \brief   Array of supported Request IDs
     **/
    const eMessageIDs    RequestIds[] = 
    {
          MSG_ID_requestChangeLight
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
          MSG_ID_responseChangeLight

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
          MSG_ID_TrafficLight
    };
    /**
     * \brief   Number of supported Attribute entries
     **/
    const unsigned int NumberofAttributes   = MACRO_ARRAYLEN(AttributeIds);

}
//////////////////////////////////////////////////////////////////////////
// NETrafficLight inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NETrafficLight methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NETrafficLight::eMessageIDs );

/************************************************************************
 * NETrafficLight::eLight enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NETrafficLight::eLight )

/**
 * Return string value of NETrafficLight::eLight
 **/
inline const char * NETrafficLight::getString( NETrafficLight::eLight enumValue )
{
    switch ( enumValue )
    {
    case    NETrafficLight::LightOff:
        return "NETrafficLight::LightOff";
    case    NETrafficLight::Red:
        return "NETrafficLight::Red";
    case    NETrafficLight::RedYellow:
        return "NETrafficLight::RedYellow";
    case    NETrafficLight::Yellow:
        return "NETrafficLight::Yellow";
    case    NETrafficLight::Green:
        return "NETrafficLight::Green";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NETrafficLight::eLight value!";
    }
}

/**
 * Returns string value of NETrafficLight::eMessageIDs
 **/
inline const char * NETrafficLight::getString( NETrafficLight::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NETrafficLight::MSG_ID_requestChangeLight:
        return "NETrafficLight::MSG_ID_requestChangeLight";
    case    NETrafficLight::MSG_ID_responseChangeLight:
        return "NETrafficLight::MSG_ID_responseChangeLight";
    case    NETrafficLight::MSG_ID_TrafficLight:
        return "NETrafficLight::MSG_ID_TrafficLight";

    case NETrafficLight::MSG_ID_NO_PROCEED:
        return "NETrafficLight::MSG_ID_NO_PROCEED";
        
    case NETrafficLight::MSG_ID_INVALID:
        return "NETrafficLight::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NETrafficLight::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NETrafficLight namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SRC_GEN_NETRAFFICLIGHT_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/NETrafficLight.hpp file
//////////////////////////////////////////////////////////////////////////
