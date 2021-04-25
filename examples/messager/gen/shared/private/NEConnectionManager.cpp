//////////////////////////////////////////////////////////////////////////
// Begin generate gen/shared/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     24.08.2019  21:12:30 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/shared/private/NEConnectionManager.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Namespace implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
 #include "gen/shared/NEConnectionManager.hpp"

//////////////////////////////////////////////////////////////////////////
// NEService::SInterfaceData structure implementation
//////////////////////////////////////////////////////////////////////////

static NEService::SInterfaceData & _getInterfaceData(void)
{
    /**
     * \brief   ConnectionManager Service Interface data
     **/
    static NEService::SInterfaceData InterfaceData;

    if (InterfaceData.idIsInitialized == false)
    {
        InterfaceData.idServiceName     = NEConnectionManager::ServiceName;
        InterfaceData.idVersion         = NEConnectionManager::InterfaceVersion;
        InterfaceData.idServiceType     = NEService::ServiceRemote;
        InterfaceData.idRequestCount    = NEConnectionManager::NumberofRequests;
        InterfaceData.idResponseCount   = NEConnectionManager::NumberofResponses;
        InterfaceData.idAttributeCount  = NEConnectionManager::NumberofAttributes;

        /**
         * \brief   The map of requests and responses
         **/
        static const unsigned int RequestToResponseMap[] = 
        {
              static_cast<unsigned int>( NEConnectionManager::MSG_ID_ResponseConnect              ) // RequestConnet( const CEString & nickName, const CEDateTime & dateTime )
            , static_cast<unsigned int>( NEConnectionManager::MSG_ID_ResponseRegisterConnection   ) // RequestRegisterConnection( const CEString & nickName, const unsigned int & cookie, const unsigned int & connectCookie, const CEDateTime & dateRegister )
            , static_cast<unsigned int>( NEConnectionManager::MSG_ID_NO_PROCEED                   ) // RequestDiconnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime )
        };

        /**
         * \brief   Initialization of map of parameter entry validation map in responses and in broadcasts
         **/
        static const unsigned int ResponseParamStateMap[] = 
        {
        /************************************************************************
         * Responses
         ************************************************************************/
            4, // void ResponseConnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime, const NEConnectionManager::eConnectionResult & result )
            3, // void ResponseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, const bool & success )

        /************************************************************************
         * Broadcasts
         ************************************************************************/
            1, // void BroadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList )
            1, // void BroadcastClientConnected( const NEConnectionManager::sConnection & clientConnected )
            1, // void BroadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft )

        };

        InterfaceData.idRequestToResponseMap    = RequestToResponseMap;
        InterfaceData.idResponseParamCountMap   = ResponseParamStateMap;
        InterfaceData.idIsInitialized           = true;
    }
    return InterfaceData;
}

//////////////////////////////////////////////////////////////////////////
// NEConnectionManager functions implementation
//////////////////////////////////////////////////////////////////////////

const NEService::SInterfaceData & NEConnectionManager::CreateInterfaceData( void )
{
    return _getInterfaceData();
}

NEConnectionManager::eMessageIDs NEConnectionManager::GetResponseId( const NEConnectionManager::eMessageIDs reqId )
{
    const NEService::SInterfaceData& sid = _getInterfaceData();

    int index = GET_REQ_INDEX(reqId);
    return  ( index >= 0 && index < static_cast<int>(sid.idRequestCount) ? static_cast<NEConnectionManager::eMessageIDs>(sid.idRequestToResponseMap[index]) : NEConnectionManager::MSG_ID_INVALID );
}

NEConnectionManager::eMessageIDs NEConnectionManager::GetRequestId( const NEConnectionManager::eMessageIDs respId )
{
    const NEService::SInterfaceData& sid = _getInterfaceData();
    NEConnectionManager::eMessageIDs result = NEConnectionManager::MSG_ID_INVALID;
    for (unsigned int i = 0; result == NEConnectionManager::MSG_ID_INVALID && i < NEConnectionManager::NumberofRequests; i ++)
        result = sid.idRequestToResponseMap[i] == static_cast<unsigned int>(respId) ? NEConnectionManager::RequestIds[i] : NEConnectionManager::MSG_ID_INVALID;
    return result;
}

//////////////////////////////////////////////////////////////////////////
// End generate gen/shared/private/NEConnectionManager.cpp file
//////////////////////////////////////////////////////////////////////////
