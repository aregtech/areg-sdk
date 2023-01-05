//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/NEConnectionManager.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "generated/NEConnectionManager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEConnectionManager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEConnectionManager::getInterfaceData( void )
{

    /************************************************************************
     * The list of requests
     ************************************************************************/
    static constexpr unsigned int _RequestList[]
    {
          static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_requestConnect               ) // requestConnect( const String & nickName, const DateTime & dateTime )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_requestRegisterConnection    ) // requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_requestDisconnect            ) // requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseList[]
    {
          static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_responseConnect              ) // responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection   ) // responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated   ) // broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected     ) // broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected  ) // broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static constexpr unsigned int _AttributeList[]
    {
          static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_ConnectionList               ) // NEConnectionManager::MapConnection mConnectionList;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static constexpr unsigned int _RequestToResponseMap[] 
    {
          static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_responseConnect              ) // requestConnect( const String & nickName, const DateTime & dateTime )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection   ) // requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
        , static_cast<unsigned int>( NEConnectionManager::eMessageIDs::MsgId_NotProcessed                 ) // requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static constexpr unsigned int _ResponseParamStateMap[]
    {
    /************************************************************************
     * Responses
     ************************************************************************/
        4, // void responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result )
        3, // void responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success )

    /************************************************************************
     * Broadcasts
     ************************************************************************/
        1, // void broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList )
        1, // void broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected )
        1, // void broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft )

    };

    /************************************************************************
     * ConnectionManager Service Interface data
     ************************************************************************/
    static NEService::SInterfaceData _InterfaceData =
    {
          NEConnectionManager::ServiceName
        , NEConnectionManager::InterfaceVersion
        , NEService::eServiceType::ServicePublic
        , 3
        , 5
        , 1
        , _RequestList
        , _ResponseList
        , _AttributeList
        , _RequestToResponseMap
        , _ResponseParamStateMap
    };

    return _InterfaceData;
}

NEConnectionManager::eMessageIDs NEConnectionManager::getResponseId( NEConnectionManager::eMessageIDs reqId)
{
    const NEService::SInterfaceData & sid = NEConnectionManager::getInterfaceData();
    msg_id index = GET_REQ_INDEX(reqId);
    
    return  ( (index >= 0) && (index < static_cast<msg_id>(sid.idRequestCount)) ? static_cast<NEConnectionManager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEConnectionManager::eMessageIDs::MsgId_Invalid );
}

NEConnectionManager::eMessageIDs NEConnectionManager::getRequestId( NEConnectionManager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEConnectionManager::getInterfaceData();
    NEConnectionManager::eMessageIDs result = NEConnectionManager::eMessageIDs::MsgId_Invalid;
    for ( unsigned int i = 0; (result == NEConnectionManager::eMessageIDs::MsgId_Invalid) && (i < sid.idRequestCount); ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<msg_id>(respId) ? static_cast<NEConnectionManager::eMessageIDs>(sid.idRequestList[i]) : NEConnectionManager::eMessageIDs::MsgId_Invalid;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////
