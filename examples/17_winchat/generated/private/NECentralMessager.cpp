//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/NECentralMessager.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/NECentralMessager.hpp"

//////////////////////////////////////////////////////////////////////////
// NECentralMessager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NECentralMessager::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NECentralMessager::eMessageIDs::MsgId_requestSendMessage         ) // requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
        , static_cast<unsigned int>( NECentralMessager::eMessageIDs::MsgId_requestKeyTyping           ) // requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage       ) // broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
        , static_cast<unsigned int>( NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping         ) // broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
        , static_cast<unsigned int>( NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage  ) // broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime )
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
          static_cast<unsigned int>( NECentralMessager::eMessageIDs::MsgId_NotProcessed               ) // requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
        , static_cast<unsigned int>( NECentralMessager::eMessageIDs::MsgId_NotProcessed               ) // requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        4, // void broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
        3, // void broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
        2, // void broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime )

    };

    /************************************************************************
     * CentralMessager Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NECentralMessager::ServiceName
        , NECentralMessager::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 2
        , 3
        , 0
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NECentralMessager::eMessageIDs NECentralMessager::getResponseId( NECentralMessager::eMessageIDs /* reqId */ )
{
    return NECentralMessager::eMessageIDs::MsgId_Invalid;
}

NECentralMessager::eMessageIDs NECentralMessager::getRequestId( NECentralMessager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NECentralMessager::getInterfaceData();
    NECentralMessager::eMessageIDs result = NECentralMessager::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NECentralMessager::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NECentralMessager::eMessageIDs>(sid.idRequestList[i]) : NECentralMessager::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////
