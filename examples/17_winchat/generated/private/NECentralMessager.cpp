//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 * Generated at     04.07.2021  04:29:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/private/NECentralMessager.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Namespace implementation.
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
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NECentralMessager::MSG_ID_requestSendMessage         ) // requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
        , static_cast<unsigned int>( NECentralMessager::MSG_ID_requestKeyTyping           ) // requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NECentralMessager::MSG_ID_broadcastSendMessage       ) // broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
        , static_cast<unsigned int>( NECentralMessager::MSG_ID_broadcastKeyTyping         ) // broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
        , static_cast<unsigned int>( NECentralMessager::MSG_ID_broadcastBroadcastMessage  ) // broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    const unsigned int * _AttributeList     = static_cast<const unsigned int *>(NULL);    /* There are no attributes. Set Invalid message ID */

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NECentralMessager::MSG_ID_NO_PROCEED                 ) // requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
        , static_cast<unsigned int>( NECentralMessager::MSG_ID_NO_PROCEED                 ) // requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
    };

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
        , NEService::ServiceRemote
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

NECentralMessager::eMessageIDs NECentralMessager::getResponseId( NECentralMessager::eMessageIDs reqId )
{
    return NECentralMessager::MSG_ID_INVALID;
}

NECentralMessager::eMessageIDs NECentralMessager::getRequestId( NECentralMessager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NECentralMessager::getInterfaceData();
    NECentralMessager::eMessageIDs result = NECentralMessager::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NECentralMessager::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NECentralMessager::eMessageIDs>(sid.idRequestList[i]) : NECentralMessager::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/NECentralMessager.cpp file
//////////////////////////////////////////////////////////////////////////
