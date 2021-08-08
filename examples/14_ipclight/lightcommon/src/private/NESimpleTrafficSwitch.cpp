//////////////////////////////////////////////////////////////////////////
// Begin generate lightcommon/src/private/NESimpleTrafficSwitch.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 * Generated at     03.08.2021  11:01:22 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            lightcommon/src/private/NESimpleTrafficSwitch.cpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "lightcommon/src/NESimpleTrafficSwitch.hpp"

//////////////////////////////////////////////////////////////////////////
// NESimpleTrafficSwitch functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NESimpleTrafficSwitch::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NESimpleTrafficSwitch::MSG_ID_requestSwitchLight  ) // requestSwitchLight( bool switchOn )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    const unsigned int * _ResponseList      = static_cast<const unsigned int *>(NULL); /* There are no responses. Set Invalid message ID */

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NESimpleTrafficSwitch::MSG_ID_IsSwitchedOn        ) // bool mIsSwitchedOn;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NESimpleTrafficSwitch::MSG_ID_NO_PROCEED          ) // requestSwitchLight( bool switchOn )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    const unsigned int * _ResponseParamStateMap = static_cast<const unsigned int *>(NULL); // EMPTY RESPONSE AND BROADCAST LIST
    
    /************************************************************************
     * SimpleTrafficSwitch Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NESimpleTrafficSwitch::ServiceName
        , NESimpleTrafficSwitch::InterfaceVersion
        , NEService::ServiceLocal
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

NESimpleTrafficSwitch::eMessageIDs NESimpleTrafficSwitch::getResponseId( NESimpleTrafficSwitch::eMessageIDs reqId )
{
    return NESimpleTrafficSwitch::MSG_ID_INVALID;
}

NESimpleTrafficSwitch::eMessageIDs NESimpleTrafficSwitch::getRequestId( NESimpleTrafficSwitch::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NESimpleTrafficSwitch::getInterfaceData();
    NESimpleTrafficSwitch::eMessageIDs result = NESimpleTrafficSwitch::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NESimpleTrafficSwitch::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NESimpleTrafficSwitch::eMessageIDs>(sid.idRequestList[i]) : NESimpleTrafficSwitch::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate lightcommon/src/private/NESimpleTrafficSwitch.cpp file
//////////////////////////////////////////////////////////////////////////
