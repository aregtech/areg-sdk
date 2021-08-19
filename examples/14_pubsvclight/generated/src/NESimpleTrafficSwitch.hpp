//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NESimpleTrafficSwitch.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NESIMPLETRAFFICSWITCH_HPP
#define  GENERATED_SRC_NESIMPLETRAFFICSWITCH_HPP

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 * Generated at     11.08.2021  13:18:21 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/NESimpleTrafficSwitch.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch Service Interface Namespace declaration.
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
// NESimpleTrafficSwitch namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   SimpleTrafficSwitch Service Interface defined constants, 
 *          types and structures message IDs, global static methods.
 *
 *          Simple traffic light switch on/off.
 *          Declared as local service to prevent any other external client to send unexpected signal to switch off the light.
 *
 **/
namespace   NESimpleTrafficSwitch
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name
     *          NESimpleTrafficSwitch::ServiceName
     **/
    const char * const    ServiceName         = "SimpleTrafficSwitch";
    /**
     * \brief   Implemented Version
     *          NESimpleTrafficSwitch::InterfaceVersion
     **/
    const Version         InterfaceVersion    = "1.0.0";

/************************************************************************/
// Service Interface data types
/************************************************************************/


/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NESimpleTrafficSwitch::eMessageIDs
     *          SimpleTrafficSwitch Service interface defined message IDs
     **/
    typedef enum E_MessageIDs
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MSG_ID_requestSwitchLight
         *          Sets the traffic light ON or OFF.
         **/
          MSG_ID_requestSwitchLight   = NEService::REQUEST_ID_FIRST     // void requestSwitchLight( bool switchOn );
     
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
         * \brief   Attribute ID: MSG_ID_IsSwitchedOn
         *          Flag that indicates the light on/off state.
         **/
        , MSG_ID_IsSwitchedOn         = NEService::ATTRIBUTE_ID_FIRST   // bool mIsSwitchedOn;
     
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
     * \brief   Returns string value of NESimpleTrafficSwitch::eMessageIDs
     **/
    inline const char * getString( NESimpleTrafficSwitch::eMessageIDs msgId );
     
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
    NESimpleTrafficSwitch::eMessageIDs getResponseId( NESimpleTrafficSwitch::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NESimpleTrafficSwitch::eMessageIDs getRequestId( NESimpleTrafficSwitch::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficSwitch inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NESimpleTrafficSwitch methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NESimpleTrafficSwitch::eMessageIDs );

/**
 * Returns string value of NESimpleTrafficSwitch::eMessageIDs
 **/
inline const char * NESimpleTrafficSwitch::getString( NESimpleTrafficSwitch::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NESimpleTrafficSwitch::MSG_ID_requestSwitchLight:
        return "NESimpleTrafficSwitch::MSG_ID_requestSwitchLight";
    case    NESimpleTrafficSwitch::MSG_ID_IsSwitchedOn:
        return "NESimpleTrafficSwitch::MSG_ID_IsSwitchedOn";

    case NESimpleTrafficSwitch::MSG_ID_NO_PROCEED:
        return "NESimpleTrafficSwitch::MSG_ID_NO_PROCEED";
        
    case NESimpleTrafficSwitch::MSG_ID_INVALID:
        return "NESimpleTrafficSwitch::MSG_ID_INVALID";
    default:
        ASSERT(false);
        return "ERROR: Undefined NESimpleTrafficSwitch::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficSwitch namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_NESIMPLETRAFFICSWITCH_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NESimpleTrafficSwitch.hpp file
//////////////////////////////////////////////////////////////////////////
