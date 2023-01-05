//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NESystemShutdown.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:08:55 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/NESystemShutdown.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NESystemShutdown.hpp"

//////////////////////////////////////////////////////////////////////////
// NESystemShutdown functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NESystemShutdown::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NESystemShutdown::eMessageIDs::MsgId_requestSystemShutdown  ) // requestSystemShutdown( void )
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
          static_cast<unsigned int>( NESystemShutdown::eMessageIDs::MsgId_ServiceState           ) // NESystemShutdown::eServiceState mServiceState;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NESystemShutdown::eMessageIDs::MsgId_NotProcessed           ) // requestSystemShutdown( void )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    constexpr unsigned int * _ResponseParamStateMap { nullptr }; // EMPTY RESPONSE AND BROADCAST LIST

    /************************************************************************
     * SystemShutdown Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NESystemShutdown::ServiceName
        , NESystemShutdown::InterfaceVersion
        , NEService::eServiceType::ServicePublic
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

NESystemShutdown::eMessageIDs NESystemShutdown::getResponseId( NESystemShutdown::eMessageIDs /* reqId */ )
{
    return NESystemShutdown::eMessageIDs::MsgId_Invalid;
}

NESystemShutdown::eMessageIDs NESystemShutdown::getRequestId( NESystemShutdown::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NESystemShutdown::getInterfaceData();
    NESystemShutdown::eMessageIDs result = NESystemShutdown::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NESystemShutdown::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NESystemShutdown::eMessageIDs>(sid.idRequestList[i]) : NESystemShutdown::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NESystemShutdown.cpp file
//////////////////////////////////////////////////////////////////////////
