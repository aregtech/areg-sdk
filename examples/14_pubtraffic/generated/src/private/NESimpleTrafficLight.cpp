//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NESimpleTrafficLight.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:10 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 *
 * \file            generated/src/NESimpleTrafficLight.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight
 *                  Service Interface namespace implementation.
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
    constexpr unsigned int * _RequestList   { nullptr }; /* There are no requests. Set Invalid message ID */

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseList  { nullptr }; /* There are no responses. Set Invalid message ID */

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth  ) // NESimpleTrafficLight::eTrafficLight mSouthNorth;
        , static_cast<unsigned int>( NESimpleTrafficLight::eMessageIDs::MsgId_EastWest    ) // NESimpleTrafficLight::eTrafficLight mEastWest;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    constexpr unsigned int * _RequestToResponseMap  { nullptr }; // The request list is empty

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseParamStateMap { nullptr }; // EMPTY RESPONSE AND BROADCAST LIST

    /************************************************************************
     * SimpleTrafficLight Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NESimpleTrafficLight::ServiceName
        , NESimpleTrafficLight::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 0
        , 0
        , 2
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NESimpleTrafficLight::eMessageIDs NESimpleTrafficLight::getResponseId( NESimpleTrafficLight::eMessageIDs /* reqId */ )
{
    return NESimpleTrafficLight::eMessageIDs::MsgId_Invalid;
}

NESimpleTrafficLight::eMessageIDs NESimpleTrafficLight::getRequestId( NESimpleTrafficLight::eMessageIDs /* respId */ )
{
    return NESimpleTrafficLight::eMessageIDs::MsgId_Invalid;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NESimpleTrafficLight.cpp file
//////////////////////////////////////////////////////////////////////////
