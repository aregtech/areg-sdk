//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NESimpleTrafficLight.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 * Generated at     11.08.2021  13:18:20 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/NESimpleTrafficLight.cpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NESimpleTrafficLight.hpp"

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficLight functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NESimpleTrafficLight::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    const unsigned int * _RequestList       = static_cast<const unsigned int *>(NULL); /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NESimpleTrafficLight::MSG_ID_broadcastLightChanged  ) // broadcastLightChanged( NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NESimpleTrafficLight::MSG_ID_SouthNorth             ) // NESimpleTrafficLight::eTrafficLight mSouthNorth;
        , static_cast<unsigned int>( NESimpleTrafficLight::MSG_ID_EastWest               ) // NESimpleTrafficLight::eTrafficLight mEastWest;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    const unsigned int * _RequestToResponseMap  = static_cast<const unsigned int *>(NULL); // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
    {
    /************************************************************************
     * Responses
     ************************************************************************/

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        2, // void broadcastLightChanged( NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest )

    };
    
    /************************************************************************
     * SimpleTrafficLight Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NESimpleTrafficLight::ServiceName
        , NESimpleTrafficLight::InterfaceVersion
        , NEService::ServiceRemote
        , 0
        , 1
        , 2
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NESimpleTrafficLight::eMessageIDs NESimpleTrafficLight::getResponseId( NESimpleTrafficLight::eMessageIDs reqId )
{
    return NESimpleTrafficLight::MSG_ID_INVALID;
}

NESimpleTrafficLight::eMessageIDs NESimpleTrafficLight::getRequestId( NESimpleTrafficLight::eMessageIDs respId )
{
    return NESimpleTrafficLight::MSG_ID_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NESimpleTrafficLight.cpp file
//////////////////////////////////////////////////////////////////////////
