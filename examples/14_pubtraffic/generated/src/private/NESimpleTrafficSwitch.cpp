//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NESimpleTrafficSwitch.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:34 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 *
 * \file            generated/src/NESimpleTrafficSwitch.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NESimpleTrafficSwitch.hpp"

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficSwitch functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NESimpleTrafficSwitch::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NESimpleTrafficSwitch::eMessageIDs::MsgId_requestSwitchLight  ) // requestSwitchLight( bool switchOn )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseList  { nullptr }; /* There are no responses. Set Invalid message ID */

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn        ) // bool mIsSwitchedOn;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NESimpleTrafficSwitch::eMessageIDs::MsgId_NotProcessed        ) // requestSwitchLight( bool switchOn )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseParamStateMap { nullptr }; // EMPTY RESPONSE AND BROADCAST LIST

    /************************************************************************
     * SimpleTrafficSwitch Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NESimpleTrafficSwitch::ServiceName
        , NESimpleTrafficSwitch::InterfaceVersion
        , NEService::eServiceType::ServiceLocal
        , 1
        , 0
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NESimpleTrafficSwitch::eMessageIDs NESimpleTrafficSwitch::getResponseId( NESimpleTrafficSwitch::eMessageIDs /* reqId */ )
{
    return NESimpleTrafficSwitch::eMessageIDs::MsgId_Invalid;
}

NESimpleTrafficSwitch::eMessageIDs NESimpleTrafficSwitch::getRequestId( NESimpleTrafficSwitch::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NESimpleTrafficSwitch::getInterfaceData();
    NESimpleTrafficSwitch::eMessageIDs result = NESimpleTrafficSwitch::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NESimpleTrafficSwitch::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NESimpleTrafficSwitch::eMessageIDs>(sid.idRequestList[i]) : NESimpleTrafficSwitch::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NESimpleTrafficSwitch.cpp file
//////////////////////////////////////////////////////////////////////////
