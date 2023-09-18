//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/NEHelloService.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:33:56 GMT+02:00
 *                  Create by AREG SDK code generator tool from source HelloService.
 *
 * \file            generated/src/NEHelloService.hpp
 * \ingroup         HelloService Service Interface
 * \brief           This is an automatic generated code of HelloService
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/src/NEHelloService.hpp"

//////////////////////////////////////////////////////////////////////////
// NEHelloService functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEHelloService::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEHelloService::eMessageIDs::MsgId_requestHelloService   ) // requestHelloService( const String & client )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEHelloService::eMessageIDs::MsgId_responseHelloService  ) // responseHelloService( bool success )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    constexpr unsigned int * _AttributeList { nullptr };    /* There are no attributes. Set Invalid message ID */

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEHelloService::eMessageIDs::MsgId_responseHelloService  ) // requestHelloService( const String & client )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        1, // void responseHelloService( bool success )

    /************************************************************************
     * Broadcasts
     ************************************************************************/

    };

    /************************************************************************
     * HelloService Service Interface data
     ************************************************************************/
    static const NEService::SInterfaceData _InterfaceData
    {
          NEHelloService::ServiceName
        , NEHelloService::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 1
        , 1
        , 0
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEHelloService::eMessageIDs NEHelloService::getResponseId( NEHelloService::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid { NEHelloService::getInterfaceData() };
    msg_id index { static_cast<msg_id>(GET_REQ_INDEX(reqId)) };

    return  ( (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEHelloService::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEHelloService::eMessageIDs::MsgId_Invalid );
}

NEHelloService::eMessageIDs NEHelloService::getRequestId( NEHelloService::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid { NEHelloService::getInterfaceData() };
    NEHelloService::eMessageIDs result { NEHelloService::eMessageIDs::MsgId_Invalid };
    for ( unsigned int i = 0; (result == NEHelloService::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEHelloService::eMessageIDs>(sid.idRequestList[i]) : NEHelloService::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/NEHelloService.cpp file
//////////////////////////////////////////////////////////////////////////
