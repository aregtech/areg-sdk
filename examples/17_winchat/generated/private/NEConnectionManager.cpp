//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     04.07.2021  04:30:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/private/NEConnectionManager.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Namespace implementation.
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
    static const unsigned int _RequestList[] = 
    {
          static_cast<unsigned int>( NEConnectionManager::MSG_ID_requestConnet                ) // requestConnet( const String & nickName, const DateTime & dateTime )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_requestRegisterConnection    ) // requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_requestDiconnect             ) // requestDiconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
    };

    /************************************************************************
     * The list of responses and broadcasts
     ************************************************************************/
    static const unsigned int _ResponseList[] = 
    {
          static_cast<unsigned int>( NEConnectionManager::MSG_ID_responseConnect              ) // responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_responseRegisterConnection   ) // responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_broadcastConnectionUpdated   ) // broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_broadcastClientConnected     ) // broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_broadcastClientDisconnected  ) // broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft )
    };

    /************************************************************************
     * The list of attributes
     ************************************************************************/
    static const unsigned int _AttributeList[] = 
    {
          static_cast<unsigned int>( NEConnectionManager::MSG_ID_ConnectionList               ) // NEConnectionManager::MapConnection mConnectionList;
    };

    /************************************************************************
     * The map of requests and responses
     ************************************************************************/
    static const unsigned int _RequestToResponseMap[] = 
    {
          static_cast<unsigned int>( NEConnectionManager::MSG_ID_responseConnect              ) // requestConnet( const String & nickName, const DateTime & dateTime )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_responseRegisterConnection   ) // requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
        , static_cast<unsigned int>( NEConnectionManager::MSG_ID_NO_PROCEED                   ) // requestDiconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
    };

    /************************************************************************
     * Initialization of parameter entry validation map in responses and in broadcasts
     ************************************************************************/
    static const unsigned int _ResponseParamStateMap[] = 
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
        , NEService::ServiceRemote
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

NEConnectionManager::eMessageIDs NEConnectionManager::getResponseId( NEConnectionManager::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = NEConnectionManager::getInterfaceData();
    int index = GET_REQ_INDEX(reqId);
    
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEConnectionManager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEConnectionManager::MSG_ID_INVALID );
}

NEConnectionManager::eMessageIDs NEConnectionManager::getRequestId( NEConnectionManager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = NEConnectionManager::getInterfaceData();
    NEConnectionManager::eMessageIDs result = NEConnectionManager::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEConnectionManager::MSG_ID_INVALID && i < sid.idRequestCount; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? static_cast<NEConnectionManager::eMessageIDs>(sid.idRequestList[i]) : NEConnectionManager::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////
