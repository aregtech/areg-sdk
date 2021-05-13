//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     12.05.2021  16:41:22 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/NEConnectionManager.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "shared/generated/NEConnectionManager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   ConnectionManager Service Interface data
     **/
    static NEService::SInterfaceData _InterfaceData;

    if (_InterfaceData.idIsInitialized == false)
    {
        _InterfaceData.idServiceName    = NEConnectionManager::ServiceName;
        _InterfaceData.idVersion        = NEConnectionManager::InterfaceVersion;
        _InterfaceData.idServiceType    = NEService::ServiceRemote;
        _InterfaceData.idRequestCount   = NEConnectionManager::NumberofRequests;
        _InterfaceData.idResponseCount  = NEConnectionManager::NumberofResponses;
        _InterfaceData.idAttributeCount = NEConnectionManager::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int _RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEConnectionManager::MSG_ID_responseConnect              ) // requestConnet( const String & nickName, const DateTime & dateTime )
            , static_cast<unsigned int>( NEConnectionManager::MSG_ID_responseRegisterConnection   ) // requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
            , static_cast<unsigned int>( NEConnectionManager::MSG_ID_NO_PROCEED                   ) // requestDiconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
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

        _InterfaceData.idRequestToResponseMap   = _RequestToResponseMap;
        _InterfaceData.idResponseParamCountMap  = _ResponseParamStateMap;
        _InterfaceData.idIsInitialized          = true;
    }
    
    return _InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEConnectionManager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEConnectionManager::createInterfaceData( void )
{
    return _getInterfaceData();
}

NEConnectionManager::eMessageIDs NEConnectionManager::getResponseId( NEConnectionManager::eMessageIDs reqId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEConnectionManager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEConnectionManager::MSG_ID_INVALID );
}

NEConnectionManager::eMessageIDs NEConnectionManager::getRequestId( NEConnectionManager::eMessageIDs respId )
{
    const NEService::SInterfaceData & sid = _getInterfaceData();
    NEConnectionManager::eMessageIDs result = NEConnectionManager::MSG_ID_INVALID;
    for ( unsigned int i = 0; result == NEConnectionManager::MSG_ID_INVALID && i < NEConnectionManager::NumberofRequests; ++ i )
    {
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NEConnectionManager::RequestIds[i] : NEConnectionManager::MSG_ID_INVALID;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////
